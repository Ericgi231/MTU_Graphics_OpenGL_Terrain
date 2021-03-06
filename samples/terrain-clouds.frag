#version 150 // GLSL 150 = OpenGL 3.2
 
out vec4 fragColor;

/* These "in" variables are interpolated from the values calculated
 * per vertex in the vertex program. They are the output of our vertex
 * program. For this program, I have named the variables so they are
 * correct in the vertex program---and they must have the same name in
 * the fragment program. */
in vec4 out_Position_CC;  // Fragment position (in cam coords)
in vec3 out_Normal_CC; // normal vector     (in cam coords)

in vec2 out_TexCoord;
uniform sampler2D texC;

/** Calculate diffuse shading. Normal and light direction do not need
 * to be normalized. */
float diffuseScalar(vec3 normal, vec3 lightDir, bool frontBackSame)
{
	/* Basic equation for diffuse shading */
	float diffuse = dot(normalize(lightDir), normalize(normal));

	/* The diffuse value will be negative if the normal is pointing in
	 * the opposite direction of the light. Set diffuse to 0 in this
	 * case. Alternatively, we could take the absolute value to light
	 * the front and back the same way. Either way, diffuse should now
	 * be a value from 0 to 1. */
	if(frontBackSame)
		diffuse = abs(diffuse);
	else
		diffuse = clamp(diffuse, 0, 1);

	/* Keep diffuse value in range from .5 to 1 to prevent any object
	 * from appearing too dark. Not technically part of diffuse
	 * shading---however, you may like the appearance of this. */
	diffuse = diffuse/2 + .5;

	return diffuse;
}

void main() 
{
	/* Get position of light in camera coordinates. When we do
	 * headlight style rendering, the light will be at the position of
	 * the camera! */
	vec3 lightPos = vec3(0,0,0);

	/* Calculate a vector pointing from our current position (in
	 * camera coordinates) to the light position. */
	vec3 lightDir = lightPos - out_Position_CC.xyz;

	/* Calculate diffuse shading */
	float diffuse = diffuseScalar(out_Normal_CC, lightDir, true);

	fragColor = vec4(1,1,1,1);
	fragColor.a = texture(texC, out_TexCoord).x;
}
