#version 330 compatibility
in vec4 vSmoothColor;
out vec4 vFragColor;

void main()
{
    //float blackWhite = mod(gl_FragCoord.y, 300);
    //if ( blackWhite < 25. )
    //{
    //    vFragColor = vec4(1.,0.,0.,1.);
    //}
    //else
    //{
    //    vFragColor = vec4(1.,0.,0.,1.);
    //}
    vFragColor = vSmoothColor;
}