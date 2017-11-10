//  CS 430 Project 2 Raycasting
//  Junzhe Wang 
//  jw2323
//  jw2323@nau.edu
//  github repository:https://github.com/Junzhewang/CS430-Project-2
//  2017/10/11

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#define CAMERA 1
#define SPHERE 2
#define PLANE 3
#define LIGHT 4


typedef struct {   // create a Object to store all the properties of sphere/plane
    int kind;
    //camera
    double width;
    double height;    // declare all the variable
    
    //sphere
    double position[3];
    double diffuse_color[3];
	double specular_color[3];
    int radius;
    
    //plane
    double normal[3];
	
	//light
	double color[3];
    double theta;
	double radial_a2;
	double radial_a1;
	double radial_a0;
	double angular_a0;
    double spotlight_direction[3];
	
} Object;

Object objects[128];   // define all variables and functions ahead
int object_count;
double camera_width ;
double camera_height ;
int light_number;
void read_csv(char *, Object*);

double plane_intersection(double[] ,double[],int);
double sphere_intersection(double[] ,double[],int);
double dot_product(double[],double[]);
double vector_length(double[]);
double point_distance(double[], double[]);
void render(int,int,char *);
int writeP3(FILE*, unsigned char*, int, int);
int writeHeader(FILE *, int, int, int, int);
double* shade(double *, int, double);
int test_intersection(int, double*, double*, double);
double* get_diff_color(double *, double *, int, int);
double* get_spec_color(double *, double *,double *, double *, int , int );
double get_radial(int ,double);
double get_angular(int ,double *);


// main method, used to check argument errors, when the input argument are wrong, return error messages.
int main(int argc, char *argv[])
{
    if(argc != 5){
        printf("Error: Please input the usage pattern like : raycast width height input.csv output.ppm.\n");
        exit(1);
    }
	if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        fprintf(stderr, "Error: main: width and height parameters must be > 0\n");
        exit(1);
    }
	const char *input = argv[3];                                 // check the error of passing arguments. for example, if the argument is empty.
    if (input == NULL) {
        fprintf(stderr, "Error: main: Failed to open input file '%s'\n", argv[3]);
        exit(1);
    }
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    printf("width:%d;height:%d\n",width,height);
    
    read_csv(argv[3], objects);   // call the read file function

	
	printf("camera: \n");
	printf("width %lf \n",objects[0].width);
	printf("height %lf \n",objects[0].height);
	printf("type %d \n",objects[0].kind);        // test the camera is correctly loaded.
	

	printf ("sphere: \n");
	printf(" kind:%lf \n",objects[1].kind);
	printf("%lf \n",objects[1].diffuse_color[0]);
	printf("%lf \n",objects[1].diffuse_color[1]);
	printf("%lf \n",objects[1].diffuse_color[2]);
	printf("%lf \n",objects[1].specular_color[0]);
	printf("%lf \n",objects[1].specular_color[1]);
	printf("%lf \n",objects[1].specular_color[2]);
	
	
	printf ("position: \n");
	printf("%lf \n",objects[1].position[0]);
	printf("%lf \n",objects[1].position[1]);
	printf("%lf \n",objects[1].position[2]);
	printf ("radius: \n");
	printf("%d \n",objects[1].radius);
	
	
	printf ("plane: \n");
	printf("kind: %d \n",objects[2].kind);
	printf("%lf \n",objects[2].diffuse_color[0]);
	printf("%lf \n",objects[2].diffuse_color[1]);
	printf("%lf \n",objects[2].diffuse_color[2]);
	
	printf ("position: \n");
	printf("%lf \n",objects[2].position[0]);
	printf("%lf \n",objects[2].position[1]);
	printf("%lf \n",objects[2].position[2]);
	printf ("normal: \n");
	printf("%lf \n",objects[2].normal[0]);
	printf("%lf \n",objects[2].normal[1]);
	printf("%lf \n",objects[2].normal[2]);
	
	printf("light:  \n");
	printf("type %d \n",objects[3].kind); 
	printf("%lf \n",objects[3].color[0]);
	printf("%lf \n",objects[3].color[1]);
	printf("%lf \n",objects[3].color[2]);
	printf("%lf \n",objects[3].theta);
	printf("%lf \n",objects[3].radial_a2);
	printf("%lf \n",objects[3].radial_a1);
	
	
	camera_height=objects[0].height;
	camera_width=objects[0].width;
	
	
    render(width,height,argv[4]);
    /*
    double Ro[3] = {0,0,0};
    double Rd[3] = {0,1,2};
    Object obj = objects[2];

    double t = plane_intersection(Ro,Rd,obj);
    double q = sphere_intersection(Ro,Rd, objects[1]);
    
    printf("%lf\n",t);
    */
    
    return 0;
}









// below are the parsing method


char next_c(FILE* csv) {   // define a function to get next character
    char c = fgetc(csv);

    return c;
}


char* next_string(FILE* csv) {    // define a function that can grab a string. 
    char buffer[129];
    char c = next_c(csv);
    int i = 0;
    while ((c != ',' )&& (c!=':')) {   // as long as c is not EOF or comma, the string is not ended.
		if (c ==EOF){
			break;
		}
        if (i >= 128) {
            fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
            exit(1);
        }
        if (c == '\\') {
            fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
            exit(1);
        }
        buffer[i] = c;
        i =i+ 1;
        c = next_c(csv);
		buffer[i]='\0';
    }
	ungetc(c,csv);  
	//printf("%s \n" , strdup(buffer));	// unget the one more character. 
	
    return strdup(buffer);
}



char* last_string(FILE* csv) {    // define a function that can grab a string. 
    char buffer[129];
    char c = next_c(csv);
    int i = 0;
    while ((c != ',' )&& (c!=':')) {   // as long as c is not EOF or comma, the string is not ended.
		if (c ==EOF){
			break;
		}
        if (i >= 128) {
            fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
            exit(1);
        }
        if (c == '\\') {
            fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
            exit(1);
        }
        buffer[i] = c;
        i =i+ 1;
        c = next_c(csv);
		
    }
	buffer[i-1]='\0';
	ungetc(c,csv);  
	//printf("%s \n" , strdup(buffer));	// unget the one more character. 
	
    return strdup(buffer);
}

char* posi_string(FILE* csv) {    // define a function that can grab a string. 
    char buffer[129];
    char c = next_c(csv);
    int i = 0;
	while (c != ']' ) {   // as long as c is not EOF or comma, the string is not ended.
		//printf("posi %c",c);
		if (c ==EOF){
			break;
		}
        if (i >= 128) {
            fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
            exit(1);
        }
        if (c == '\\') {
            fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
            exit(1);
        }
        buffer[i] = c;
        i =i+ 1;
        c = next_c(csv);
		
    }
	buffer[i]='\0';
	 
	printf("%s \n" , strdup(buffer));	// unget the one more character. 
	//printf("$$$$$$$%f$$$$",atof(strdup(buffer)));
    return strdup(buffer);
}



double next_number(FILE* csv) {    // define a function to grab numbers.
    double value;
    int res = fscanf(csv, "%lf", &value);
    if (res == EOF) {
        fprintf(stderr, "Error: Expected a number but found EOF.");
        exit(1);
    }
    // printf("next_number: %lf\n", value);
    return value;
}


void skipWS(FILE* csv){
	char c = next_c(csv);
	while (isspace(c)){
		c = next_c(csv);
	}
	ungetc(c,csv);
	// check and skip white space in the csv file
}

void lookfor(FILE* csv, char d){    // locate the wanted character and move the pointer to it.
	while(!next_c(csv)==d){
				char c = next_c(csv);
			}
}




void read_csv(char *input_file, Object* objects){
    char buffer[1024];
    FILE *csv = fopen(input_file,"r");
    if(csv == NULL)
    {
        printf("ERROR: file not open!");
		exit(1);
    }
    int line = 0; 
	int counter= 0;

	skipWS(csv);// skip white space
	
	char firstLetter = next_c(csv);
	if (firstLetter!='c'){
		printf("Error: CSV file shouold start with the word camera.");
		exit(1);
	}
	ungetc(firstLetter,csv);   // move the pointer backward
	// Check if the file start with camera object.
	
	
    while(1){
		char *type = next_string(csv);
		printf("%s %d\n",type,counter); 
		if (strcmp(type, "camera")==0){
			(objects+counter)->kind=CAMERA;
		}
		if (strcmp(type, "sphere")==0){
			(objects+counter)->kind=SPHERE;
		}
		if (strcmp(type,"plane")==0){
			(objects+counter)->kind=PLANE;
		}
		if (strcmp(type,"light")==0){
			(objects+counter)->kind=LIGHT;
			light_number=counter;
		}
		lookfor(csv,',');
		skipWS(csv);
		
		// reading the type
		
		
		
		
		
		char *wid = next_string(csv);
		//printf("%s %d\n",wid,counter);
		if (strcmp(wid,"width")==0){
			skipWS(csv);
			lookfor(csv,':');
			skipWS(csv);
			double width = next_number(csv);
			
			//printf("%lf \n" ,width);
			
			
			(objects+counter)->width=width;			
			lookfor(csv,',');
			
			
			skipWS(csv);
			char *tall = next_string(csv);
			if (strcmp(tall,"height")==0){
				lookfor(csv,':');
				skipWS(csv);
				double hei = next_number(csv);
				
				//printf("%lf \n",hei);
				
				(objects+counter)->height=hei;

						
			}
					
		}
		// finish parsing camera
		
		
		if(strcmp(wid,"radius")==0){
			lookfor(csv,':');
			skipWS(csv);
			double rad = next_number(csv);
				
				//printf("%lf \n",hei);
				
			(objects+counter)->radius=rad;  //read in radius 
			
			
			lookfor(csv,',');
			skipWS(csv);
			
			char *diffuse = next_string(csv);
			//printf("%s",diffuse);
			if (strcmp(diffuse,"diffuse_color")==0)
				lookfor(csv,'[');
				skipWS(csv);
				
				next_c(csv);
				(objects+counter)->diffuse_color[0]=atof(next_string(csv));
				skipWS(csv);
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->diffuse_color[1]=atof(next_string(csv));
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->diffuse_color[2]=atof(last_string(csv));
				skipWS(csv);
				lookfor(csv,',');
				skipWS(csv);  // read in the diffuse_color parameters
				
				
				
				char *t = next_string(csv);
				//printf("%s \n",t);
				if (strcmp(t,"specular_color")==0){
				
				lookfor(csv,'[');
				skipWS(csv);
				
				next_c(csv);
				(objects+counter)->specular_color[0]=atof(next_string(csv));
				skipWS(csv);
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->specular_color[1]=atof(next_string(csv));
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->specular_color[2]=atof(last_string(csv));
				// read in the specular_color parameters
				}else {
					printf("Error: can't find the specular color");
					exit(1);
				}
			skipWS(csv);
			lookfor(csv,',');
			skipWS(csv);
			char *c = next_string(csv);
			//printf("%s \n",c);
			if (strcmp(c,"position")==0){
				
				lookfor(csv,'[');
				skipWS(csv);
				next_c(csv);
				(objects+counter)->position[0]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->position[1]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->position[2]=atof(posi_string(csv));
			}else {
				printf("Error: can't find the sphere position");
				exit(1);
			}
			
		}
		// finish parsing sphere
		
		
		if (strcmp(wid,"normal")==0){
			lookfor(csv,'[');
			skipWS(csv);
			
			next_c(csv);
			(objects+counter)->normal[0]=atof(next_string(csv));
			skipWS(csv);
			lookfor(csv,',');
			skipWS(csv);
			(objects+counter)->normal[1]=atof(next_string(csv));
			lookfor(csv,',');
			skipWS(csv);
			(objects+counter)->normal[2]=atof(last_string(csv));
			skipWS(csv);
			lookfor(csv,',');
			skipWS(csv);
			
			char *dif = next_string(csv);
			if (strcmp(dif,"diffuse_color")==0)
				lookfor(csv,'[');
				skipWS(csv);
				
				next_c(csv);
				(objects+counter)->diffuse_color[0]=atof(next_string(csv));
				skipWS(csv);
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->diffuse_color[1]=atof(next_string(csv));
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->diffuse_color[2]=atof(last_string(csv));
				skipWS(csv);
				lookfor(csv,',');
				skipWS(csv);  // read in the diffuse_color parameters
				
				
				
				char *posit = next_string(csv);
				if (strcmp(posit,"position")==0){
				
				lookfor(csv,'[');
				skipWS(csv);
				next_c(csv);
				(objects+counter)->position[0]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->position[1]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->position[2]=atof(posi_string(csv));
				}else {
					printf("Error: can't find the plane position");
					exit(1);
				}
		}
		// finish parsing plane
		
		
		if (strcmp(wid,"color")==0){
			//printf("go into the loop"); 
			lookfor(csv,'[');
			skipWS(csv);
			
			next_c(csv);
			(objects+counter)->color[0]=atof(next_string(csv));
			skipWS(csv);
			lookfor(csv,',');
			skipWS(csv);
			(objects+counter)->color[1]=atof(next_string(csv));
			lookfor(csv,',');
			skipWS(csv);
			(objects+counter)->color[2]=atof(last_string(csv));
			skipWS(csv);
			lookfor(csv,',');
			skipWS(csv);
			while (1){
			char *theta = next_string(csv);
			//printf("%s",theta);
			if (strcmp(theta,"theta")==0){
			skipWS(csv);
			lookfor(csv,':');
			skipWS(csv);
			double the = next_number(csv);
			(objects+counter)->theta=the;			
			lookfor(csv,',');
			skipWS(csv);
			}
			
			else if (strcmp(theta,"radial-a2")==0){
			skipWS(csv);
			lookfor(csv,':');
			skipWS(csv);
			double a2 = next_number(csv);
			(objects+counter)->radial_a2=a2;			
			lookfor(csv,',');			
			skipWS(csv);
			}
			
			
			else if (strcmp(theta,"radial-a1")==0){
			skipWS(csv);
			lookfor(csv,':');
			skipWS(csv);
			double a1 = next_number(csv);
			(objects+counter)->radial_a1=a1;			
			lookfor(csv,',');			
			skipWS(csv);
			}
			
			
			else if (strcmp(theta,"radial-a0")==0){
			skipWS(csv);
			lookfor(csv,':');
			skipWS(csv);
			double a0 = next_number(csv);
			(objects+counter)->radial_a0=a0;			
			lookfor(csv,',');			
			skipWS(csv);
			}
			
			//printf("%s \n",last);
			else if (strcmp(theta,"position")==0){
				
				lookfor(csv,'[');
				skipWS(csv);
				next_c(csv);
				(objects+counter)->position[0]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->position[1]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->position[2]=atof(last_string(csv));
			}
			else if (strcmp(theta,"direction")==0){
				
				lookfor(csv,'[');
				skipWS(csv);
				next_c(csv);
				(objects+counter)->spotlight_direction[0]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->spotlight_direction[1]=atof(next_string(csv));
				
				lookfor(csv,',');
				skipWS(csv);
				(objects+counter)->spotlight_direction[2]=atof(last_string(csv));
			}
			else if (strcmp(theta,"angular-a0")==0){
			skipWS(csv);
			lookfor(csv,':');
			skipWS(csv);
			double a0 = next_number(csv);
			(objects+counter)->angular_a0=a0;			
			lookfor(csv,',');			
			skipWS(csv);
			} 
			else {
				break;
			}
			}
		}
		// finish parsing light
		
		
		skipWS(csv);
		char g = next_c(csv);
		if (g==EOF){
			break;
		}
		ungetc(g,csv);
		skipWS(csv);	
		counter= counter+1;
		
		
	}
    fclose(csv);
	object_count= counter+1;

}



double *raycast(double Ro[3], double Rd[3]){
    

    
    int closest_obj_index = 0;
    double closest_t = -1;
    for(int i = 0; i < object_count; i ++) {
                
        double t = -1;
        if(objects[i].kind == SPHERE){
            t = sphere_intersection(Ro, Rd, i);
        }
        
        if(objects[i].kind == PLANE){
            t = plane_intersection(Ro,Rd,i);
        }
        
        if(closest_t == -1 && t != -1){
            closest_t = t;
            closest_obj_index = i;
        } else
        if(t != -1 && t < closest_t){
            closest_t = t;
            closest_obj_index = i;
        }
    }
    
    static double color[3];
    static double *color2;
    
    if(closest_obj_index == 0) {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    } else {
        color2 = shade(Rd, closest_obj_index, closest_t);
        return color2;
    }
    return color;
}

int test_intersection(int self_index, double Ro[3], double Rd[3], double test) {
    
    double nor_Rd[3];
    nor_Rd[0] = Rd[0] / vector_length(Rd);
    nor_Rd[1] = Rd[1] / vector_length(Rd);
    nor_Rd[2] = Rd[2] / vector_length(Rd);

    
    double best_t = -1;
    int best_o = -1;
    
    
    for (int i=0; i < object_count; i++) {
        if (self_index == i) continue;
        
        if(i == 3) continue;

        double t = 0;
        switch(objects[i].kind) {
            case CAMERA:
                break;
            case LIGHT:
                break;
            case SPHERE:
                t = sphere_intersection(Ro, nor_Rd, i);
                break;
            case PLANE:
                t = plane_intersection(Ro, nor_Rd, i);
                break;
        }

        if (test != -1 && t > test){
            continue;}
        if ((t > 0 && t < best_t )|| (t > 0 && best_t == -1)) {
            best_t = t;
            best_o = i;
        }
     
    }
    return best_o;
}


double* shade(double shoot_direction[3], int closest_obj_index, double t){
    
    static double reflect_color[3];
    
    double shooting_point[3];
    shooting_point[0] = t * shoot_direction[0];
    shooting_point[1] = t * shoot_direction[1];
    shooting_point[2] = t * shoot_direction[2];
    
    double light_direction[3];
    light_direction[0] = objects[light_number].position[0] - shooting_point[0];
    light_direction[1] = objects[light_number].position[1] - shooting_point[1];
    light_direction[2] = objects[light_number].position[2] - shooting_point[2];
    
    double light_length = sqrt(light_direction[0]*light_direction[0] + light_direction[1]*light_direction[1] + light_direction[2]*light_direction[2]);
    
    int test = test_intersection(closest_obj_index,shooting_point,light_direction,light_length);
    
    if(test != -1){
        reflect_color[0] = 0;
        reflect_color[1] = 0;
        reflect_color[2] = 0;
        return reflect_color;
    } else {
        //finding normal vector
        double normal[3];
        
        if(objects[closest_obj_index].kind == PLANE) {
            normal[0] = objects[closest_obj_index].normal[0];
            normal[1] = objects[closest_obj_index].normal[1];
            normal[2] = objects[closest_obj_index].normal[2];
        } else if (objects[closest_obj_index].kind == SPHERE ) {
            normal[0] = shooting_point[0] - objects[closest_obj_index].position[0];
            normal[1] = shooting_point[1] - objects[closest_obj_index].position[1];
            normal[2] = shooting_point[2] - objects[closest_obj_index].position[2];
        }
        
        double N[3];
        N[0] = normal[0] / vector_length(normal);
        N[1] = normal[1] / vector_length(normal);
        N[2] = normal[2] / vector_length(normal);

        double L[3];
        L[0] = light_direction[0] / vector_length(light_direction);
        L[1] = light_direction[1] / vector_length(light_direction);
        L[2] = light_direction[2] / vector_length(light_direction);
        
        double V[3];
        V[0] = shoot_direction[0] / vector_length(shoot_direction);
        V[1] = shoot_direction[1] / vector_length(shoot_direction);
        V[2] = shoot_direction[2] / vector_length(shoot_direction);
        
        double R[3];
        R[0] = L[0] - (N[0] * 2.0 * dot_product(N, L));
        R[1] = L[1] - (N[1] * 2.0 * dot_product(N, L));
        R[2] = L[2] - (N[2] * 2.0 * dot_product(N, L));
        
        double* diff_color;

        diff_color = get_diff_color(N, L, light_number, closest_obj_index);
        
        double* spec_color;
        spec_color = get_spec_color(L,R,N,V,light_number, closest_obj_index);
        
        double radial = get_radial(light_number,light_length);
        double angular =  get_angular(light_number, L);

        reflect_color[0] = (diff_color[0] + spec_color[0]) * radial * angular;
        reflect_color[1] = (diff_color[1] + spec_color[1]) * radial * angular;
        reflect_color[2] = (diff_color[2] + spec_color[2]) * radial * angular;
        
        return reflect_color;
    }
}

double* get_diff_color(double *normal, double *light_direction, int light_number, int object) {
    
    static double color[3];

    if (dot_product(normal,light_direction) > 0) {
        color[0] = objects[light_number].color[0] * objects[object].diffuse_color[0] * dot_product(normal,light_direction);
        color[1] = objects[light_number].color[1] * objects[object].diffuse_color[1] * dot_product(normal,light_direction);
        color[2] = objects[light_number].color[2] * objects[object].diffuse_color[2] * dot_product(normal,light_direction);
    }
    else {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }
    return color;
}

double* get_spec_color(double *light_direction, double *reflect_light_direction,double *normal, double *view_vector, int light_number, int object) {

    static double color[3];
    if (dot_product(normal,light_direction) > 0 && dot_product(view_vector,reflect_light_direction) > 0) {
        double index = pow(dot_product(view_vector,reflect_light_direction), 20);
        color[0] = objects[light_number].color[0] * objects[object].specular_color[0] * index;
        color[1] = objects[light_number].color[1] * objects[object].specular_color[1] * index;
        color[2] = objects[light_number].color[2] * objects[object].specular_color[2] * index;
    }
    else {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }
    return color;
}

double get_radial(int light_number,double dist) {
    double dist_sqr = dist * dist;
    return 1 / (objects[light_number].radial_a2 * dist_sqr + objects[light_number].radial_a1 * dist + objects[light_number].angular_a0);
}

double get_angular(int light_number,double *direction_to_object){
    if(objects[light_number].theta == 0){
        return 1.0;
    }

    double theta_rad = objects[light_number].theta * (M_PI / 180);
    double cos_theta = cos(theta_rad);
    double vo_dot_vl = dot_product(objects[light_number].spotlight_direction, direction_to_object);
    if (vo_dot_vl < cos_theta){
        return 0.0;
    }
    return pow(vo_dot_vl, objects[light_number].angular_a0);
}



void render(int width,int height,char *output_file) {
    
    double pixheight = camera_height / height;
    double pixwidth = camera_width / width;
    double Pij[3];
    unsigned char img[width*height*3];
    int temp[width*height*3];
    int ptr = 0;
    
    for(int i = 0; i < width; i ++){
        Pij[1] =  -(0 - camera_height/2.0 + pixheight*(i + 0.5));
        for(int j = 0; j < height; j ++){
            Pij[0] = 0 - camera_width/2.0 + pixwidth*(j + 0.5);
            Pij[2] = -1;
            
            double Ro[3] = {0,0,0};

            double Rd[3];
            Rd[0] = Pij[0] / vector_length(Pij);
            Rd[1] = Pij[1] / vector_length(Pij);
            Rd[2] = Pij[2] / vector_length(Pij);

            double *color;
            
            color = raycast(Ro,Rd);
            if(color[0] > 1){
                color[0] = 1;
            }
            if(color[1] > 1){
                color[1] = 1;
            }
            if(color[2] > 1){
                color[2] = 1;
            }
            
            if(color[0] < 0){
                color[0] = 0;
            }
            if(color[1] < 0){
                color[1] = 0;
            }
            if(color[2] < 0){
                color[2] = 0;
            }
            temp[ptr++] = (int)(color[0] * 255);
            temp[ptr++] = (int)(color[1] * 255);
            temp[ptr++] = (int)(color[2] * 255);
        }
    }
    
    for(int i = 0; i < width*height*3; i++){
        img[i] = (unsigned char)temp[i];
    }
    
    //output p3.ppm
    FILE *outfile = fopen(output_file, "w");
    if (outfile==NULL) {
        fprintf(stderr,"Error: FILE error\n");
        exit(-1);
    }
    writeHeader(outfile, 3, width, height, 255);
    writeP3(outfile, img, width, height);
    fclose(outfile);
}

double plane_intersection(double Ro[3],double Rd[3],int obj){
    double alph,delta;
    
    double normal[3];
    normal[0] = objects[obj].normal[0] / vector_length(objects[obj].normal);
    normal[1] = objects[obj].normal[1] / vector_length(objects[obj].normal);
    normal[2] = objects[obj].normal[2] / vector_length(objects[obj].normal);
    
    alph = dot_product(normal, Rd);
    
    // the plane is parallel to the ray
    if (fabs(alph) <0.0001) {
        return -1;
    }
    
    double incident_vector[3];
    incident_vector[0] = objects[obj].position[0] - Ro[0];
    incident_vector[1] = objects[obj].position[1] - Ro[1];
    incident_vector[2] = objects[obj].position[2] - Ro[2];

    delta = dot_product(incident_vector, normal);
    
    double t = delta/alph;
    
    if (t<0.0) {
        return -1;
    }
    return t;
}


double sphere_intersection(double Ro[3],double Rd[3],int obj) {
    double a, b;
    double vector_diff[3];
    vector_diff[0] = Ro[0] - objects[obj].position[0];
    vector_diff[1] = Ro[1] - objects[obj].position[1];
    vector_diff[2] = Ro[2] - objects[obj].position[2];
    
    // calculate quadratic formula
    a = 2 * (Rd[0]*vector_diff[0] + Rd[1]*vector_diff[1] + Rd[2]*vector_diff[2]);
    b = vector_diff[0]*vector_diff[0] + vector_diff[1]*vector_diff[1] + vector_diff[2]*vector_diff[2] - objects[obj].radius*objects[obj].radius;
    
    // check that discriminant is <, =, or > 0
    double disc = a*a - 4*b;
    double t;  // solutions
    if (disc < 0) {
        return -1; // no solution
    }
    disc = sqrt(disc);
    t = (-a - disc) / 2.0;
    if (t < 0.0)
        t = (-a + disc) / 2.0;
    
    if (t < 0.0)
        return -1;
    
    return t;
}

//vector calculation
double dot_product(double x[3],double y[3]){
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}

double vector_length(double x[3]){
    return sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
}

double point_distance(double x[3], double y[3]){
    return sqrt((x[0]-y[0])*(x[0]-y[0]) + (x[1]-y[1])*(x[1]-y[1]) + (x[2]-y[2])*(x[2]-y[2]));
}


//output file (Project 1)
int writeHeader(FILE *f, int magicNum, int w, int h, int color) {
    if (f==NULL) {
        fprintf(stderr,"Error: file doesn't exist\n");
        exit(-1);
    }
    
    if (magicNum==3) {
        fprintf(f, "P3\n");
    } else if (magicNum==6) {
        fprintf(f, "P6\n");
    } else {
        fprintf(stderr,"Error: magicNum can only be P3 or P6\n");
        exit(-1);
    }
    
    fprintf(f, "%d %d\n", w, h);
    fprintf(f, "%d\n", color);
    return 0;
}

int writeP3(FILE* f, unsigned char* img, int w, int h) {
    int i,j;
    for(i=0;i<h;i++) {
        for(j=0;j<w;j++) {
            fprintf(f, "%d ",img[i*w*3+3*j]);
            fprintf(f, "%d ",img[i*w*3+3*j+1]);
            fprintf(f, "%d ",img[i*w*3+3*j+2]);
        }
        fprintf(f, "\n");
    }
    return 0;
}
