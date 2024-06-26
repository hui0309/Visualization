#version 430
layout (points) in;
layout (triangle_strip, max_vertices = 65) out;

in VS_OUT {
    vec4 color;
} gs_in[];
out vec4 COLOR;

void main() { 
    COLOR = gs_in[0].color;
    int seg = 32;
    float theta = 2.0 * 3.1415926 / float(seg);
    float angle = 0.0f;
    float radius = 0.01f;
    for(int i = 0; i <= seg; ++i) {
        // edge
        gl_Position = gl_in[0].gl_Position + vec4(cos(angle) * radius, sin(angle) * radius, 0.0, 0.0);
        EmitVertex();
        
        // center of the circle
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        
        // Update angle
        angle += theta;
    }
    EndPrimitive();
}
