#version 330 core

precision mediump float;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D vTexture;
uniform int width;
uniform int height;
uniform vec4 params;
uniform float brightness;


vec2 blurCoordinates[24];
vec3 W = vec3(0.299, 0.587, 0.114);

const mat3 saturateMatrix = mat3(
    1.1102, -0.0598, -0.061,
    -0.0774, 1.0826, -0.1186,
     -0.0228, -0.0228, 1.1772
);


float hardLight(float color){

    if(color <= 0.5){
        color = color * color * 2.0;
    }
    else{
        color = 1.0 - ((1.0 - color)*(1.0 - color) * 2.0);
    }
    return color;
}


void main(){
    // 1. 模糊: 平滑处理 步长
    vec2 singleStepOffest = vec2(1.0/float(width), 1.0/float(height));

    blurCoordinates[0] = TexCoord.xy + singleStepOffest * vec2(0.0, -10.0);
    blurCoordinates[1] = TexCoord.xy + singleStepOffest * vec2(0.0, 10.0);
    blurCoordinates[2] = TexCoord.xy + singleStepOffest * vec2(-10.0, 0.0);
    blurCoordinates[3] = TexCoord.xy + singleStepOffest * vec2(10.0, 0.0);


    blurCoordinates[4] = TexCoord.xy + singleStepOffest * vec2(5.0, -8.0);
    blurCoordinates[5] = TexCoord.xy + singleStepOffest * vec2(5.0, 8.0);
    blurCoordinates[6] = TexCoord.xy + singleStepOffest * vec2(-8.0, 5.0);
    blurCoordinates[7] = TexCoord.xy + singleStepOffest * vec2(8.0, 5.0);


    blurCoordinates[8] = TexCoord.xy + singleStepOffest * vec2(8.0, -5.0);
    blurCoordinates[9] = TexCoord.xy + singleStepOffest * vec2(8.0, 5.0);
    blurCoordinates[10] = TexCoord.xy + singleStepOffest * vec2(-5.0, 8.0);
    blurCoordinates[11] = TexCoord.xy + singleStepOffest * vec2(5.0, 8.0);
    blurCoordinates[12] = TexCoord.xy + singleStepOffest * vec2(0.0, -6.0);
    blurCoordinates[13] = TexCoord.xy + singleStepOffest * vec2(0.0, 6.0);
    blurCoordinates[14] = TexCoord.xy + singleStepOffest * vec2(-6.0, 0.0);
    blurCoordinates[15] = TexCoord.xy + singleStepOffest * vec2(6.0, 0.0);

    blurCoordinates[16] = TexCoord.xy + singleStepOffest * vec2(-4.0, -4.0);
    blurCoordinates[17] = TexCoord.xy + singleStepOffest * vec2(-4.0, 4.0);
    blurCoordinates[18] = TexCoord.xy + singleStepOffest * vec2(4.0, -4.0);
    blurCoordinates[19] = TexCoord.xy + singleStepOffest * vec2(4.0, 4.0);

    blurCoordinates[20] = TexCoord.xy + singleStepOffest * vec2(-2.0, -2.0);
    blurCoordinates[21] = TexCoord.xy + singleStepOffest * vec2(-2.0, 2.0);
    blurCoordinates[22] = TexCoord.xy + singleStepOffest * vec2(2.0, -2.0);
    blurCoordinates[23] = TexCoord.xy + singleStepOffest * vec2(2.0, 2.0);


    // 获取中心点坐标
    vec3 currentColor = texture2D(vTexture, TexCoord).rgb;
    //vec3 totalRGB = currentColor.rgb;
    float sampleColor = currentColor.g * 22.0;
    for (int i = 0; i < 24; i++){
        if(i <= 11){
            sampleColor += texture2D(vTexture, blurCoordinates[i]).g;
        }
        else if(i <= 19){
            sampleColor += texture2D(vTexture, blurCoordinates[i]).g * 2.0;
        }
        else{
            sampleColor += texture2D(vTexture, blurCoordinates[i]).g * 3.0;
        }
    }
    sampleColor = sampleColor / 62.0;

    float highPass = currentColor.g - sampleColor + 0.5;
    for(int i = 0; i < 5; i++){
        highPass = hardLight(highPass);
    }

    float lumance = dot(currentColor, W);
    float alpha = pow(lumance, params.r);
    vec3 smoothColor = currentColor + (currentColor-vec3(highPass))*alpha*0.1;

    smoothColor.r = clamp(pow(smoothColor.r, params.g), 0.0, 1.0);
    smoothColor.g = clamp(pow(smoothColor.g, params.g), 0.0, 1.0);
    smoothColor.b = clamp(pow(smoothColor.b, params.g), 0.0, 1.0);

    vec3 lvse = vec3(1.0)-(vec3(1.0)-smoothColor)*(vec3(1.0)-currentColor);
    vec3 bianliang = max(smoothColor, currentColor);
    vec3 rouguang = 2.0*currentColor*smoothColor + currentColor*currentColor - 2.0*currentColor*currentColor*smoothColor;

    FragColor = vec4(mix(currentColor, lvse, alpha), 1.0);
    FragColor.rgb = mix(FragColor.rgb, bianliang, alpha);
    FragColor.rgb = mix(FragColor.rgb, rouguang, params.b);
    vec3 satcolor = FragColor.rgb * saturateMatrix;
    FragColor.rgb = mix(FragColor.rgb, satcolor, params.a);
    FragColor.rgb = vec3(FragColor.rgb + vec3(brightness));

    //-----------------------------//

    //vec4 blur = vec4(totalRGB * 1.0 / 21.0, currentColor.a);
    // 高通道颜色
    //vec4 highPassColor = currentColor - blur;

    // 反向
    //highPassColor.r = clamp(2.0 * highPassColor.r * highPassColor.r * 24.0, 0.0, 1.0);
    //highPassColor.g = clamp(2.0 * highPassColor.g * highPassColor.g * 24.0, 0.0, 1.0);
    //highPassColor.b = clamp(2.0 * highPassColor.b * highPassColor.b * 24.0, 0.0, 1.0);


    //vec3 rgb = mix(currentColor.rgb, blur.rgb, 0.2);

    //FragColor = vec4(rgb,1.0);

}
