#version 330 core
#define PI 3.14159265359

out vec4 FragColor;
in vec3 color;
in vec2 texCoords;
in vec3 Normal;
in vec3 crntPosition;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPos;
uniform vec2 iResolution;
uniform float iTime;

//Approx for the acos function found at https://www.shadertoy.com/view/lsjXDc
float acos( float x )
{
    float y = abs( clamp(x,-1.0,1.0) );
    float z = (-0.168577*y + 1.56723) * sqrt(1.0 - y);
    return mix( 0.5*3.1415927, z, sign(x) );
}

//Preethan Sky Model found at https://www.shadertoy.com/view/llSSDR
float saturatedDot(in vec3 a, in vec3 b){
    return max(dot(a, b), 0.0);
}

vec3 computeSphericalCoordinates(in vec2 uv){
    float theta = uv.x * 2.0 * PI;
    float phi   = uv.y * PI;
	return vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)); 
}

vec3 YxyToXYZ( in vec3 Yxy )
{
	float Y = Yxy.r;
	float x = Yxy.g;
	float y = Yxy.b;

	float X = x * ( Y / y );
	float Z = ( 1.0 - x - y ) * ( Y / y );

	return vec3(X,Y,Z);
}

vec3 XYZToRGB( in vec3 XYZ )
{
	// CIE/E
	mat3 M = mat3
	(
		 2.3706743, -0.9000405, -0.4706338,
		-0.5138850,  1.4253036,  0.0885814,
 		 0.0052982, -0.0146949,  1.0093968
	);

	return XYZ * M;
}


vec3 YxyToRGB( in vec3 Yxy )
{
	vec3 XYZ = YxyToXYZ( Yxy );
	vec3 RGB = XYZToRGB( XYZ );
	return RGB;
}

void calculatePerezDistribution( in float t, out vec3 A, out vec3 B, out vec3 C, out vec3 D, out vec3 E )
{
	A = vec3(  0.1787 * t - 1.4630, -0.0193 * t - 0.2592, -0.0167 * t - 0.2608 );
	B = vec3( -0.3554 * t + 0.4275, -0.0665 * t + 0.0008, -0.0950 * t + 0.0092 );
	C = vec3( -0.0227 * t + 5.3251, -0.0004 * t + 0.2125, -0.0079 * t + 0.2102 );
	D = vec3(  0.1206 * t - 2.5771, -0.0641 * t - 0.8989, -0.0441 * t - 1.6537 );
	E = vec3( -0.0670 * t + 0.3703, -0.0033 * t + 0.0452, -0.0109 * t + 0.0529 );
}

vec3 calculateZenithLuminanceYxy( in float t, in float thetaS )
{
	float chi  	 	= ( 4.0 / 9.0 - t / 120.0 ) * ( PI - 2.0 * thetaS );
	float Yz   	 	= ( 4.0453 * t - 4.9710 ) * tan( chi ) - 0.2155 * t + 2.4192;

	float theta2 	= thetaS * thetaS;
    float theta3 	= theta2 * thetaS;
    float T 	 	= t;
    float T2 	 	= t * t;

	float xz =
      ( 0.00165 * theta3 - 0.00375 * theta2 + 0.00209 * thetaS + 0.0)     * T2 +
      (-0.02903 * theta3 + 0.06377 * theta2 - 0.03202 * thetaS + 0.00394) * T +
      ( 0.11693 * theta3 - 0.21196 * theta2 + 0.06052 * thetaS + 0.25886);

    float yz =
      ( 0.00275 * theta3 - 0.00610 * theta2 + 0.00317 * thetaS + 0.0)     * T2 +
      (-0.04214 * theta3 + 0.08970 * theta2 - 0.04153 * thetaS + 0.00516) * T +
      ( 0.15346 * theta3 - 0.26756 * theta2 + 0.06670 * thetaS + 0.26688);

	return vec3( Yz, xz, yz );
}

vec3 calculatePerezLuminanceYxy( in float theta, in float gamma, in vec3 A, in vec3 B, in vec3 C, in vec3 D, in vec3 E )
{
	return ( 1.0 + A * exp( B / cos( theta ) ) ) * ( 1.0 + C * exp( D * gamma ) + E * cos( gamma ) * cos( gamma ) );
}

vec3 calculateSkyLuminanceRGB( in vec3 s, in vec3 e, in float t )
{
	vec3 A, B, C, D, E;
	calculatePerezDistribution( t, A, B, C, D, E );

	float thetaS = acos( saturatedDot( s, vec3(0,1,0) ) );
	float thetaE = acos( saturatedDot( e, vec3(0,1,0) ) );
	float gammaE = acos( saturatedDot( s, e )		   );

	vec3 Yz = calculateZenithLuminanceYxy( t, thetaS );

	vec3 fThetaGamma = calculatePerezLuminanceYxy( thetaE, gammaE, A, B, C, D, E );
	vec3 fZeroThetaS = calculatePerezLuminanceYxy( 0.0,    thetaS, A, B, C, D, E );

	vec3 Yp = Yz * ( fThetaGamma / fZeroThetaS );

	return YxyToRGB( Yp );
}


vec4 preethamSkyModel(){
    vec2 uv             = texCoords.xy;
    
    float turbidity     = 2.0;
    float azimuth       = iTime * 0.125;
    float inclination   = iTime * 0.25;
    vec3 sunDir     	= normalize( vec3( sin( inclination ) * cos( azimuth ), cos( inclination ), sin( inclination ) * sin(azimuth) ) );
    vec3 viewDir  		= -computeSphericalCoordinates( uv ).xzy;
    vec3 skyLuminance 	= calculateSkyLuminanceRGB( sunDir, viewDir, turbidity );
    
    return vec4( skyLuminance * 0.05, 1.0 );

}


vec4 pointLight(){
    vec3 lightVec = lightPosition - crntPosition;
    float lightDist = length(lightVec);
    float a = 1;
    float b = 1;
    float intensity = 1.0f/(a*lightDist*lightDist + b*lightDist + 1.0f);

    float ambient = 0.01f;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVec);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - crntPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    //Increasing the power has the effect of making the specular light more point like
    float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return texture(diffuse0, texCoords) * lightColor * (diffuse * intensity + ambient) + texture(specular0, texCoords).r * specular;
}


vec4 directionalLight(){
    float ambient = 0.20f;
    vec3 normal = normalize(Normal);

    //Specify the direction in the opposite direction you want the light to affect.
    //Eg if you want the directional light coming DOWN from ABOVE
    //   you need to specify the light direction as straight up
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - crntPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    //Increasing the power has the effect of making the specular light more point like
    float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return texture(diffuse0, texCoords) * lightColor * (diffuse + ambient) + texture(specular0, texCoords).r * specular;
}


vec4 spotLight(){

    //We write these values as the cosine of the angle, instead of the angle itself.
    //cosine calculations are slow. Womp Womp :(

    float outerCone = 0.90f;
    float innerCone = 0.95f;


    float ambient = 0.20f;
    vec3 normal = normalize(Normal);

    vec3 lightDirection = normalize(lightPosition - crntPosition);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - crntPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    //Increasing the power has the effect of making the specular light more point like
    float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    float intensity = clamp((angle - outerCone)/(innerCone - outerCone), 0.0f, 1.0f);

    return texture(diffuse0, texCoords) * lightColor * (diffuse*intensity + ambient) + texture(specular0, texCoords).r * specular*intensity;
}

vec4 returnDiffuseTexture(){
    return texture(diffuse0, texCoords);
}

vec4 returnSpecularTexture(){
    return texture(specular0, texCoords);
}


void main(){
    FragColor = directionalLight() * preethamSkyModel();
};