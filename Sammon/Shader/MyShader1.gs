#version 430
layout (points) in;
layout (triangle_strip, max_vertices = 65) out;

in VS_OUT {
    vec4 color;
    vec2 axixX;
    vec2 axixY;
} gs_in[];
out vec4 COLOR;

void main() { 
    COLOR = gs_in[0].color;
    int seg = 64;
    float theta = 2.0 * 3.1415926 / float(seg);
    float angle = 0.0f;
    for(int i = 0; i <= seg; ++i) {
        // edge
        float x = gl_in[0].gl_Position + cos(angle) * gs_in[0].axisX.x + sin(angle) * gs_in[0].axisY.x;
        float y = gl_in[0].gl_Position + cos(angle) * gs_in[0].axisX.y + sin(angle) * gs_in[0].axisY.y;
        gl_Position = vec4(x, y, 0.0, 1.0);
        EmitVertex();
        // Update angle
        angle += theta;
    }
    EndPrimitive();
}
