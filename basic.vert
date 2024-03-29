#version 330 core //Koju verziju GLSL jezika da koristim (za programabilni pipeline: GL verzija * 100) i koji profil (core - samo programabilni pipeline, compatibility - core + zastarjele stvari)

//Kanali (in, out, uniform)
layout(location = 0) in vec3 inPos; //Pozicija tjemena
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout(location = 3) in vec4 inCol; //Boja tjemena - ovo saljemo u fragment sejder
layout(location = 4) in vec2 inTex;   // Texture coordinates

out vec4 channelCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera
out vec2 TexCoords;
out vec3 chFragPos;
out vec3 chNormal;


uniform mat4 uM; //Matrica transformacije
uniform mat4 uV; //Matrica kamere
uniform mat4 uP; //Matrica projekcija


uniform int mode; //0 index, 1 models, 2 islands, 3 clouds, 4 fishes, 5 fire
uniform float seaPos;
uniform float firePos;

uniform mat4 rotationMatrix;
uniform mat4 translationMatrix;
uniform float scale;

void main() 
{
    switch (mode) {
        case 0:
            gl_Position = vec4(inPos, 1.0);
            TexCoords = inTex;
            break;
        
        case 1:
            TexCoords = inUV;
            mat4 scaledModel = uM * mat4(vec4(scale, 0.0, 0.0, 0.0),
                                        vec4(0.0, scale, 0.0, 0.0),
                                        vec4(0.0, 0.0, scale, 0.0),
                                        vec4(0.0, 0.0, 0.0, 1.0));

            mat4 finalModel = translationMatrix * scaledModel;
            chFragPos = vec3(finalModel * vec4(inPos.x, inPos.y + seaPos, inPos.z, 1.0));
            chNormal = mat3(transpose(inverse(finalModel))) * inNormal;
            gl_Position = uP * uV * vec4(chFragPos, 1.0);
            break; 
        
        case 2:
           TexCoords = inUV;
            scaledModel = uM * mat4(vec4(scale, 0.0, 0.0, 0.0),
                                        vec4(0.0, scale, 0.0, 0.0),
                                        vec4(0.0, 0.0, scale, 0.0),
                                        vec4(0.0, 0.0, 0.0, 1.0));
            finalModel = translationMatrix * scaledModel;
            vec3 newPos = inPos;
            if (inPos.y < 0.5){
                 newPos = vec3(inPos.x, inPos.y - 3.0, inPos.z);   
            }
            chFragPos = vec3(finalModel * vec4(newPos, 1.0));   
            chNormal = mat3(transpose(inverse(finalModel))) * inNormal;
            gl_Position = uP * uV * vec4(chFragPos, 1.0);
            break;
        
        case 3:
            TexCoords = inUV;
            scaledModel = uM * mat4(vec4(scale, 0.0, 0.0, 0.0),
                                        vec4(0.0, scale, 0.0, 0.0),
                                        vec4(0.0, 0.0, scale, 0.0),
                                        vec4(0.0, 0.0, 0.0, 1.0));
            finalModel = translationMatrix * rotationMatrix * scaledModel;
            chFragPos = vec3(finalModel * vec4(inPos.x, inPos.y + seaPos*30.0, inPos.z, 1.0));
            chNormal = mat3(transpose(inverse(finalModel))) * inNormal;
            gl_Position = uP * uV * vec4(chFragPos, 1.0);
            channelCol = vec4(0.2, 0.2, 0.2, 1.0);
            break;      
               
        case 4:
            TexCoords = inUV;
            scaledModel = uM * mat4(vec4(scale, 0.0, 0.0, 0.0),
                                        vec4(0.0, scale, 0.0, 0.0),
                                        vec4(0.0, 0.0, scale, 0.0),
                                        vec4(0.0, 0.0, 0.0, 1.0));
            finalModel = translationMatrix * scaledModel;
            chFragPos = vec3(finalModel * vec4(inPos, 1.0));
            chNormal = mat3(transpose(inverse(finalModel))) * inNormal;
            gl_Position = uP * uV * vec4(chFragPos, 1.0);
            channelCol = vec4(1.0, 1.0, 1.0, 0.5);
            break; 

        case 5:

            TexCoords = inUV;
            scaledModel = uM * mat4(vec4(scale, 0.0, 0.0, 0.0),
                                        vec4(0.0, scale, 0.0, 0.0),
                                        vec4(0.0, 0.0, scale, 0.0),
                                        vec4(0.0, 0.0, 0.0, 1.0));

            finalModel = translationMatrix * scaledModel;
            chFragPos = vec3(finalModel * vec4(inPos.x, inPos.y + seaPos, inPos.z, 1.0));
            chNormal = mat3(transpose(inverse(finalModel))) * inNormal;

            vec3 targetPos = vec3(1.3, 0.0, 1.3);

            float displacement = (chFragPos.y - targetPos.y) * firePos;
            if(chFragPos.y < 0.3){
                displacement = chFragPos.y;
            }
            gl_Position = uP * uV * vec4(chFragPos.x, displacement, chFragPos.z, 1.0);
            vec4 redColor = vec4(1.0, 0.0, 0.0, 1.0);
            vec4 yellowColor = vec4(1.0, 1.0, 0.0, 1.0);
            float distance = length(chFragPos - targetPos);
            channelCol = mix(yellowColor, redColor, smoothstep(firePos, 0, distance));
            break;
        
        default:
            TexCoords = inUV;
            chFragPos = vec3(uM * vec4(inPos, 1.0));
            chNormal = mat3(transpose(inverse(uM))) * inNormal;  
            gl_Position = uP * uV * vec4(inPos, 1.0);
            channelCol = inCol;
    }
}
