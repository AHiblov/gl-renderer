// The MIT License
// Copyright © 2020 Xavier Chermain (ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Implementation of
// Procedural Physically based BRDF for Real-Time Rendering of Glints
// 2020 Xavier Chermain (ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Accepted for [Pacific Graphic 2020](https://pg2020.org/) and for CGF special issue.

//Adopted by Artem Hiblov for the gl-renderer project

#version 330

uniform sampler2D colourTexture;

uniform mat3 rotation;
uniform vec3 lightDirection;

uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;
uniform vec3 cameraPosition;
uniform vec2 materialAlpha; //Glitter area adjustment. [0.01 - 1]

const float m_pi = 3.141592;
const float m_i_sqrt_2 = 0.707106; //1/sqrt(2)

const float DictAlpha = 0.5;
const int DictNLevels = 16;
const int DictN = 64 * 3; //64 is numberOfDistributionsPerChannel
const float MicrofacetRelativeArea = 0.1; //0.01 - 1
const float MaterialLogMicrofacetDensity = 15.0; //15 - 40

in vec3 passPositionWld;
in vec2 passUv;
in vec3 passNormalWld;
in vec3 passTangentWld;
in vec3 passBitangentWld;

out vec4 colour;

//Beckmann distribution
float p22_beckmann_anisotropic(float x, float y, float alpha_x, float alpha_y)
{
	float x_sqr = x * x;
	float y_sqr = y * y;
	float sigma_x = alpha_x * m_i_sqrt_2;
	float sigma_y = alpha_y * m_i_sqrt_2;
	float sigma_x_sqr = sigma_x * sigma_x;
	float sigma_y_sqr = sigma_y * sigma_y;
	return exp(-0.5 * ((x_sqr / sigma_x_sqr) + (y_sqr / sigma_y_sqr))) / (2.0 * m_pi * sigma_x * sigma_y);
}

float ndf_beckmann_anisotropic(vec3 omega_h, float alpha_x, float alpha_y)
{
	float slope_x = -(omega_h.x / omega_h.z);
	float slope_y = -(omega_h.y / omega_h.z);
	float cos_theta = omega_h.z;
	float cos_2_theta = cos_theta * cos_theta;
	float cos_4_theta = cos_2_theta * cos_2_theta;
	float beckmann_p22 = p22_beckmann_anisotropic(slope_x, slope_y, alpha_x, alpha_y);
	return beckmann_p22 / cos_4_theta;
}

//https://www.shadertoy.com/view/llGSzw
float hashIQ(uint n)
{
    // integer hash copied from Hugo Elias
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return float(n & 0x7fffffffU) / float(0x7fffffff);
}

//===== Sampling from a normal distribution =====
float sampleNormalDistribution(float U, float mu, float sigma)
{
    float x = sigma * 1.414213f /** erfinv(2.0f * U - 1.0f)*/ + mu;
    return x;
}

float normalDistribution1D(float x, float mean, float std_dev)
{
	float xMinusMean = x - mean;
	float xMinusMeanSqr = xMinusMean * xMinusMean;
	return exp(-xMinusMeanSqr / (2. * std_dev * std_dev)) / (std_dev * 2.506628); // 2.506628 \approx sqrt(2 * \pi)
}

//===== Evaluation of the i th marginal distribution P =====
//===== with slope x and at LOD level. Procedural version =====

float P_procedural(float x, int i, int level)
{
	// We use even functions
	x = abs(x);
	// After 4 standard deviation sigma, we consider that the distribution equals zero
	float sigma_dist_4 = 4.0 * 0.5 / 1.414214; // alpha_dist = 0.5 so sigma_dist \approx 0.3535 (0.5 / sqrt(2)). 0.5 = AlphaDic
	if(x >= sigma_dist_4)
		return 0.;
    
	int nMicrofacetsCurrentLevel = int(pow(2., float(level)));
	float density = 0.;
	// Dictionary should be precomputed, but we cannot use memory with Shadertoy
	// So we generate it on the fly with a very limited number of lobes
	nMicrofacetsCurrentLevel = min(32, nMicrofacetsCurrentLevel);
    
	for(int n = 0; n < nMicrofacetsCurrentLevel; ++n)
	{
		float U_n = hashIQ(uint(i*7333+n*5741));
		// alpha roughness equals sqrt(2) * RMS roughness
		//     ALPHA_DIC     =   1.414214 * std_dev
		// std_dev = ALPHA_DIC / 1.414214 
		float currentMean = sampleNormalDistribution(U_n, 0.0, 0.5 / 1.414214); //0.5 = AlphaDic
		density += normalDistribution1D(x, currentMean, 0.05) +
					normalDistribution1D(-x, currentMean, 0.05);
	}
	// 0.5 comes from that in each loop iteration, we sum two PDFs.
	// This value is essential for P_procedural to be a PDF (integral equal one).
	return density / float(nMicrofacetsCurrentLevel) * 0.5;
}

//===== Spatially-varying, multiscale, rotated, and scaled slope distribution function =====
//===== Eq. 11, Alg. 3 =====
float P22_theta_alpha(vec2 slope_h, int l, int s0, int t0)
{
	// Coherent index
	// Eq. 8, Alg. 3, line 1
	int twoToTheL = int(pow(2.0, float(l)));
	s0 *= twoToTheL;
	t0 *= twoToTheL;

	// Seed pseudo random generator
	// Alg. 3, line 2
	uint rngSeed = uint(s0 + 1549 * t0);

	// Alg.3, line 3
	float uMicrofacetRelativeArea = hashIQ(rngSeed * 13U);
	// Discard cells by using microfacet relative area
	// Alg.3, line 4
	if (uMicrofacetRelativeArea > MicrofacetRelativeArea)
		return 0.0f;

	// Number of microfacets in a cell
	// Alg. 3, line 5
	float n = pow(2.0, float(2 * l - (2 * (DictNLevels - 1))));
	n *= exp(MaterialLogMicrofacetDensity);

	// Corresponding continuous distribution LOD
	// Alg. 3, line 6
	float l_dist = log(n) / 1.38629; // 2. * log(2) = 1.38629

	// Alg. 3, line 7
	float uDensityRandomisation = hashIQ(rngSeed * 2171U);

	// Fix density randomisation to 2 to have better appearance
	// Notation in the paper: \zeta
	float densityRandomisation = 2.0;

	// Sample a Gaussian to randomise the distribution LOD around the distribution level l_dist
	// Alg. 3, line 8
	l_dist = sampleNormalDistribution(uDensityRandomisation, l_dist, densityRandomisation);

	// Alg. 3, line 9
	int l_disti = clamp(int(round(l_dist)), 0, DictNLevels);

	// Alg. 3, line 10
	if (l_disti == DictNLevels)
		return p22_beckmann_anisotropic(slope_h.x, slope_h.y, materialAlpha.x, materialAlpha.y);

	// Alg. 3, line 13
	float uTheta = hashIQ(rngSeed);
	float theta = 2.0 * m_pi * uTheta;

	float cosTheta = cos(theta);
	float sinTheta = sin(theta);

	vec2 scaleFactor = vec2(materialAlpha.x / DictAlpha,
							materialAlpha.y / DictAlpha);

	// Rotate and scale slope
	// Alg. 3, line 16
	slope_h = vec2(slope_h.x * cosTheta / scaleFactor.x + slope_h.y * sinTheta / scaleFactor.y,
				   -slope_h.x * sinTheta / scaleFactor.x + slope_h.y * cosTheta / scaleFactor.y);

	vec2 abs_slope_h = vec2(abs(slope_h.x), abs(slope_h.y));

	int distPerChannel = DictN / 3;
	float alpha_dist_isqrt2_4 = DictAlpha * m_i_sqrt_2 * 4.0;

	if (abs_slope_h.x > alpha_dist_isqrt2_4 || abs_slope_h.y > alpha_dist_isqrt2_4)
		return 0.0;

	// Alg. 3, line 17
	float u1 = hashIQ(rngSeed * 16807U);
	float u2 = hashIQ(rngSeed * 48271U);

	// Alg. 3, line 18
	int i = int(u1 * float(DictN));
	int j = int(u2 * float(DictN));

	// 3 distributions values in one texel
	int distIdxXOver3 = i / 3;
	int distIdxYOver3 = j / 3;

	float texCoordX = abs_slope_h.x / alpha_dist_isqrt2_4;
	float texCoordY = abs_slope_h.y / alpha_dist_isqrt2_4;

	float P_i = P_procedural(slope_h.x, i, l_disti);
    float P_j = P_procedural(slope_h.y, j, l_disti);

    // Alg. 3, line 19
    return P_i * P_j / (scaleFactor.x * scaleFactor.y);
}

//===== Alg. 2, P-SDF for a discrete LOD =====
// Go through cells within the pixel footprint for a givin LOD
float P22__P_(int l, vec2 slope_h, vec2 st, vec2 dst0, vec2 dst1)
{
	// Convert surface coordinates to appropriate scale for level
	float pyrSize = int(pow(2., float(DictNLevels - 1 - l))); //Pyramid size at LOD level
	st[0] = st[0] * pyrSize - 0.5f;
	st[1] = st[1] * pyrSize - 0.5f;
	dst0[0] *= pyrSize;
	dst0[1] *= pyrSize;
	dst1[0] *= pyrSize;
	dst1[1] *= pyrSize;

	// Compute ellipse coefficients to bound filter region
	float A = dst0[1] * dst0[1] + dst1[1] * dst1[1] + 1.0;
	float B = -2.0 * (dst0[0] * dst0[1] + dst1[0] * dst1[1]);
	float C = dst0[0] * dst0[0] + dst1[0] * dst1[0] + 1.0;
	float invF = 1.0 / (A * C - B * B * 0.25f);
	A *= invF;
	B *= invF;
	C *= invF;
	
	// Compute the ellipse's bounding box in texture space
	float det = -B * B + 4.0 * A * C;
	float invDet = 1.0 / det;
	float uSqrt = sqrt(det * C), vSqrt = sqrt(A * det);
	int s0 = int(ceil(st[0] - 2.0 * invDet * uSqrt));
	int s1 = int(floor(st[0] + 2.0 * invDet * uSqrt));
	int t0 = int(ceil(st[1] - 2.0 * invDet * vSqrt));
	int t1 = int(floor(st[1] + 2.0 * invDet * vSqrt));
	
	// Scan over ellipse bound and compute quadratic equation
	float sum = 0.0f;
	float sumWts = 0.0;
	int nbrOfIter = 0;
	for(int it = t0; it <= t1; ++it)
	{
		float tt = it - st[1];
		for(int is = s0; is <= s1; ++is)
		{
			float ss = is - st[0];
			// Compute squared radius and filter SDF if inside ellipse
			float r2 = A * ss * ss + B * ss * tt + C * tt * tt;
			if(r2 < 1.0)
			{
				// Weighting function used in pbrt-v3 EWA function
				float alpha = 2.0;
				float W_P = exp(-alpha * r2) - exp(-alpha);
				// Alg. 2, line 3
				sum += P22_theta_alpha(slope_h, l, is, it) * W_P;
				sumWts += W_P;
			}
			nbrOfIter++;
			// Guardrail (Extremely rare case.)
			if(nbrOfIter > 100)
				break;
		}
		// Guardrail (Extremely rare case.)
		if(nbrOfIter > 100)
			break;
	}
	
	return sum / sumWts;
}

//===== Evaluation of our procedural physically based glinty BRDF =====
//===== Alg. 1, Eq. 14 =====
vec3 f_P(vec3 wo, vec3 wi)
{
	if (wo.z <= 0.0)
		return vec3(0.0, 0.0, 0.0);
	if (wi.z <= 0.0)
		return vec3(0.0, 0.0, 0.0);

	// Alg. 1, line 1
	vec3 wh = normalize(wo + wi);
	if (wh.z <= 0.0)
		return vec3(0.0, 0.0, 0.0);

	// Local masking shadowing
	if (dot(wo, wh) <= 0.0 || dot(wi, wh) <= 0.0)
		return vec3(0.0);
	
	// Eq. 1, Alg. 1, line 2
	vec2 slope_h = vec2(-wh.x / wh.z, -wh.y / wh.z);
	
	float D_P = 0.0;
	float P22_P = 0.0;
	
	// Alg. 1, line 3
	vec2 dst0 = dFdx(passUv);
	vec2 dst1 = dFdy(passUv);

	// Compute ellipse minor and major axes
	float dst0LengthSquared = dst0.x * dst0.x + dst0.y * dst0.y;
	float dst1LengthSquared = dst1.x * dst1.x + dst1.y * dst1.y;
	
	if(dst0LengthSquared < dst1LengthSquared)
	{
		// Swap dst0 and dst1
		vec2 tmp = dst0;
		float tmpF = dst0LengthSquared;

		dst0 = dst1;
		dst1 = tmp;

		dst0LengthSquared = dst1LengthSquared;
		dst1LengthSquared = tmpF;
    }
	float majorLength = sqrt(dst0LengthSquared);
	// Alg. 1, line 5
	float minorLength = sqrt(dst1LengthSquared);
	
	const float MaxAnisotropy = 8.0;
	
	// Clamp ellipse eccentricity if too large
	// Alg. 1, line 4
	if (minorLength * MaxAnisotropy < majorLength && minorLength > 0.)
	{
		float scale = majorLength / (minorLength * MaxAnisotropy);
		dst1 *= scale;
		minorLength *= scale;
	}
	// ----------------------------------

	// Without footprint, we evaluate the Cook Torrance BRDF
	if (minorLength == 0.0)
	{
		D_P = ndf_beckmann_anisotropic(wh, materialAlpha.x, materialAlpha.y);
	}
	else
	{
		// Choose LOD
		// Alg. 1, line 6
		float l = max(0.0, DictNLevels - 1.0 + log2(minorLength));
		int il = int(floor(l));

		// Alg. 1, line 7
		float w = l - float(il);

		// Alg. 1, line 8
		P22_P = mix(P22__P_(il, slope_h, passUv, dst0, dst1),
					P22__P_(il + 1, slope_h, passUv, dst0, dst1),
					w);

		// Eq. 6, Alg. 1, line 10
		D_P = P22_P / (wh.z * wh.z * wh.z * wh.z);
	}

	// V-cavity masking shadowing
	float G1wowh = min(1.0, 2.0 * wh.z * wo.z / dot(wo, wh));
	float G1wiwh = min(1.0, 2.0 * wh.z * wi.z / dot(wi, wh));
	float G = G1wowh * G1wiwh;

	// Fresnel is set to one for simplicity here
	// but feel free to use "real" Fresnel term
	vec3 F = vec3(1.0, 1.0, 1.0);

	// Eq. 14, Alg. 1, line 14
	// (wi dot wg) is cancelled by
	// the cosine weight in the rendering equation
	return (F * G * D_P) / (4.0 * wo.z);
}

void main()
{
	vec4 textureDiffuseColour = texture(colourTexture, passUv);
	vec4 textureAmbientColour = textureDiffuseColour * vec4(0.25, 0.25, 0.25, 1.0);
	
	// Matrix for transformation to tangent space
	mat3 toLocal = mat3(
		passTangentWld.x, passBitangentWld.x, passNormalWld.x,
		passTangentWld.y, passBitangentWld.y, passNormalWld.y,
		passTangentWld.z, passBitangentWld.z, passNormalWld.z);

	vec3 lightDirectionTang = normalize(toLocal * normalize(-lightDirection)); //"wi" in original shader
	vec3 viewDirection = normalize(toLocal * normalize(cameraPosition - passPositionWld)); //"wo" in original shader
	
	vec3 radianceSpecular = f_P(viewDirection, lightDirectionTang);

	// Gamma
	radianceSpecular = pow(radianceSpecular, vec3(1.0 / 2.2));

	colour = textureAmbientColour * vec4(ambientLightColour, 1.0) +
		textureDiffuseColour * clamp(dot(passNormalWld, -lightDirection), 0.0, 1.0) * vec4(diffuseLightColour, 1.0) +
		vec4(radianceSpecular, 1.0);
}