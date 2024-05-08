#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

// NOTE: Render size values should be passed from code
const float renderWidth = 800;
const float renderHeight = 450;

float radius = 250.0;
float angle = 0.8;

uniform vec2 center = vec2(200.0, 200.0);

void main()
{
    // Get size of the render texture
    vec2 texSize = vec2(renderWidth, renderHeight);

    // Get texture coordinates, will be used to calculate the offset
    vec2 tc = fragTexCoord*texSize;

    // Subtract the center of the effect from the texture coordinates
    tc -= center;

    // Calculate the linear distance from the center of the effect
    float dist = length(tc);

    // If the distance is less than the radius of the effect, apply the distortion
    if (dist < radius)
    {
        // Calculate the percentage of the distance between the center of the effect and the edge of the effect
        float percent = (radius - dist)/radius;

        // Calculate the amount of swirling using a sinusoidal function and multipliers
        float theta = percent*percent*angle*8.0;
        float s = sin(theta);
        float c = cos(theta);

        // Calculate the final offset using matrix multiplication
        // The matrix multiplication below is a 2D rotation matrix, which rotates the texture
        // coordinates around the center of the effect by the amount specified in the angle
        // variable. The values of c and s are the cosine and sine of the angle, respectively.
        // The dot product is used to perform the matrix multiplication because it is more
        // efficient than the traditional method of multiplying two matrices.
        // The resulting texture coordinates are stored in the tc variable and will be used
        // to sample the texture in the next step.
        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
    }

    // Add the center of the effect back to the texture coordinates
    tc += center;

    // Sample the texture using the corrected texture coordinates and the original color
    vec4 color = texture2D(texture0, tc/texSize)*colDiffuse*fragColor;;

    // Output the final color with full opacity
    finalColor = vec4(color.rgb, 1.0);;
}
