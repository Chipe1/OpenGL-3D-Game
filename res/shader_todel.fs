#version 330 core

in vec2 uv;
in vec3 pos_world;
in vec3 normal_cam;
in vec3 eye_cam;
in vec3 light_cam;

uniform sampler2D tex;
uniform vec3 light_pos;

out vec3 color;

void main(){
	vec3 light_color=vec3(1,1,1);
	float light_power=50;

	vec3 diff_color=texture(tex,uv).rgb;
	vec3 amb_color=vec3(0.1,0.1,0.1)*diff_color;
	vec3 spec_color=vec3(0.2,0.2,0.2);

	float dist=length(light_pos-pos_world);
	vec3 n=normalize(normal_cam);
	vec3 l=normalize(light_cam);
	float cos_theta=clamp(dot(n,l),0,1);

	vec3 e=normalize(eye_cam);
	vec3 r=reflect(-l,n);
	float cos_alpha=clamp(dot(e,r),0,1);

	color=amb_color+diff_color*light_color*light_power*cos_theta/(dist*dist)+spec_color*light_color*light_power*pow(cos_alpha,5)/(dist*dist);
}
