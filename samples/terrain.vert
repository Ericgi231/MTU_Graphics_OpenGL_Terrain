#version 150 // GLSL 150 = OpenGL 3.2

/* Vertex position and normal vector from the C program. These are in
 * object coordinates. */
in vec3 in_Position;
in vec3 in_Normal;

/* We output a position and normal in "camera coordinates = CC". After
 * the vertex program is run, these are interpolated across the
 * object, so they are no longer the vertex position and normal in the
 * fragment program...they are the fragment's position and normal. */
out vec4 out_Position_CC;
out vec3 out_Normal_CC;
out vec2 out_TexCoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform sampler2D texE;
uniform sampler2D texT;
uniform int showNorm;

void main() 
{
	float factor = 1;
	out_TexCoord = vec2(in_Position.x, -in_Position.z);
	vec2 t1 = vec2(in_Position.x+0.001, -in_Position.z);
	vec2 t2 = vec2(in_Position.x, -in_Position.z-0.001);

	float color1 = texture(texE, out_TexCoord).x;
	float bump1 = (color1/factor);
	float color2 = texture(texE, t1).x;
	float bump2 = (color2/factor);
	float color3 = texture(texE, t2).x;
	float bump3 = (color3/factor);

	vec3 origMat = vec3(in_Position.x, bump1, in_Position.z);
	vec3 upMat = vec3(in_Position.x+0.001, bump2, in_Position.z);
	vec3 rightMat = vec3(in_Position.x, bump3, in_Position.z-0.001);

	vec3 u1 = vec3(origMat.x - upMat.x, origMat.y - upMat.y, origMat.z - upMat.z);
	vec3 u2 = vec3(origMat.x - rightMat.x, origMat.y - rightMat.y, origMat.z - rightMat.z);
	vec3 n1 = cross(u1, u2);
	
	// Construct a normal matrix from the ModelView matrix. The
	// modelview is the same for all vertices, the normal matrix is
	// too. It would be more efficient to calculate it in our C
	// program once for this object. However, it is easier to
	// calculate here.
	mat3 NormalMat = transpose(inverse(mat3(ModelView)));
	
	// Transform the normal by the NormalMat and send it to the
	// fragment program.

	out_Normal_CC = normalize(NormalMat * n1);

	// Transform the vertex position from object coordinates into
	// camera coordinates.
	out_Position_CC = ModelView * vec4(origMat, 1);

	// Transform the vertex position from object coordinates into
	// Normalized Device Coordinates (NDC).
	gl_Position = Projection * out_Position_CC;
}
