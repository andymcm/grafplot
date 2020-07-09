#include <graphics.h>
#include <stdio.h>
#include <ctype.h>
#include <alloc.h>
#include <dos.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <values.h>

#define NO 0
#define YES 1
#define ERROR 2
#define ALL 0
#define XYAXES 1
#define FORMULA 2
#define XAXIS 3
#define YAXIS 4
#define FUNCTIONS 14
#define PI 3.14159265358979323846


int display_menu( int x, int y, char *options, int characters_width );
int wait_for_input();
int set_up_display( int graphdriver );

save_graph( int ask_for_file_name );
char *summarise_data();
unsigned int hash_total();

print_graph( int number_of_pins );
print_dots( int number_of_pins, int dot_one, int dot_two, int dot_three );

int QUITmenu();

int change_numbers_to_letters( unsigned char *formula );
int abbreviate_functions( unsigned char *formula );
int create_formula_table( int *f_array );
int pick_out_sub_section( int *f_array, int *sub_section );
int translate_sub_section( int *sub_section );
int translate_operators( int *sub_section, int operator_one, int operator_two );
int translate_functions( int *sub_section );
int translate_formula( unsigned char *formula, char *error_message );
int test_the_formula( char *error_message );
add_multiply_operator( unsigned char *formula );

prompt_user_for_data( int data_required, double *new_xminimum,
							 double *new_xmaximum, double *new_yminimum,
							 double *new_ymaximum );

draw_graph( double xmin, double xmax, double ymin, double ymax );
plot_points( int left, int right, int top, int bottom );
double calculate_point( double x );
double divide( double x, double y );
double power( double x, double y );
double sine( double x );
double cosine( double x );
double tangent( double x );
double arcsin( double x );
double arccos( double x );
double arctan( double x );
double logbase10( double x );
double logbaseE( double x );
double squareroot( double x );

display_error_message( int x, int y, char *message, char *endword );
int display_warning_message( int x1, int x2, char *line_one, char *line_two );
int point_not_defined_error();

text_input( char *prompt, int x, int y, int maximum_characters, char *reply );
box( int borderlines, int xco1, int yco1, int xco2, int yco2 );
char * sub_string( char *string, int start_position, int number_of_characters );
int check_for_digits( char *x );
int check_for_accuracy( double minimum, double maximum );
convert_double_to_string( double number, char *string, int decimal_places );



long int xratio, yratio;
char *sub_string_store;
char file_name[50] = "\0";
double number_table[100], f_t_values[100];
int f_t_pointer=0;
double minimum_x, division_x, minimum_y, division_y;
char original_formula[100]="";
int speed=5, point_defined_flag=YES, radians_flag = NO, data_saved_flag=YES;
int grid_flag=YES, mouse_installed=YES;
double coordinate_stack[20][4];
int coordinate_stack_pointer = 0;

struct formula_table
{
	int operator;
	int leftvalue;
	int rightvalue;
} f_table[100];

char function_table[FUNCTIONS][6] = { 's', 'i', 'n', '$', '$', '$',
												  'c', 'o', 's', '$', '$', '$',
												  't', 'a', 'n', '$', '$', '$',
												  'a', 's', 'i', 'n', '$', '$',
												  'a', 'c', 'o', 's', '$', '$',
												  'a', 't', 'a', 'n', '$', '$',
												  's', 'i', 'n', 'h', '$', '$',
												  'c', 'o', 's', 'h', '$', '$',
												  't', 'a', 'n', 'h', '$', '$',
												  'a', 'b', 's', '$', '$', '$',
												  'l', 'o', 'g', '$', '$', '$',
												  'l', 'n', '$', '$', '$', '$',
												  'e', 'x', 'p', '$', '$', '$',
												  's', 'q', 'r', 't', '$', '$' };

