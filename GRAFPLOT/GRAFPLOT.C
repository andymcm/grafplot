#include "grafplot.h"


main()
{
	set_up_routine();

	menu_system();
}

set_up_routine()
{
	set_up_display( ask_for_display() );

	menu_headings();
	initialise_mouse();
}

menu_system()
{

	while( wait_for_input() );

	end();

}

int ask_for_display()
{
	char adaptor;

	do
	{
		printf( "\nWhich display adaptor (EGA/VGA) ? " );
		scanf( "%c", &adaptor );
		adaptor = toupper( adaptor );
	}
	while( adaptor != 'E' && adaptor != 'V'  );


	if( adaptor == 'E' )
	{
		xratio = 640;
		yratio = 350;
		return EGA;
	}

	if( adaptor == 'V' )
	{
		xratio = 640;
		yratio = 480;
		return VGA;
	}
}

set_up_display( int graph_driver )
{
	int graph_mode;


	if( graph_driver == EGA )
	{
		registerbgidriver( EGAVGA_driver );
		graph_driver = EGA;
		graph_mode = EGAHI;
	}

	if( graph_driver == VGA )
	{
		registerbgidriver( EGAVGA_driver );
		graph_driver = VGA;
		graph_mode = VGAHI;
	}


	initgraph( &graph_driver, &graph_mode, "" );


	if( graph_driver < 0 )
	{
		printf( "\n\nYou do not have the required graphics hardware\n" );
		end();
	}


	setfillstyle( SOLID_FILL, LIGHTBLUE );
	bar( 0,0, xratio-1, yratio-1 );
}


end()
{
	closegraph();

	exit(0);
}

menu_headings()
{
	setcolor( WHITE );


	outtextxy( 10*xratio/640,  10*yratio/480, "File" );
	outtextxy( 110*xratio/640, 10*yratio/480, "Graph" );
	outtextxy( 210*xratio/640, 10*yratio/480, "Print" );
	outtextxy( 310*xratio/640, 10*yratio/480, "Options" );
	outtextxy( 410*xratio/640, 10*yratio/480, "Zoom" );
	outtextxy( 510*xratio/640, 10*yratio/480, "Quit" );


	setcolor( WHITE );
	moveto( 0, (15*yratio/480)+textheight("T") );
	lineto( xratio-1, (15*yratio/480)+textheight("T") );

	moveto( 0, (18*yratio/480)+textheight("T") );
	lineto( xratio-1, (18*yratio/480)+textheight("T") );

}


int display_menu( int x, int y, char *options, int characters_width )
{
	int loop=0, position_counter=0, pixel_height, pixel_width;
	int number_of_options, position_flag=0, valid_input_flag=NO;
	char *graphics_buffer;
	char option_key[30];
	int mouse_x, mouse_y, return_value=-1, mouse_test=NO, key_test;
	union REGS inregs, outregs;


	hide_mouse();


	while( *(options+loop) != -1 )
	{
		loop += characters_width;
	}


	number_of_options = loop/characters_width;


	pixel_height = textheight("T")*(number_of_options+2);
	pixel_width  = textwidth("T")*(characters_width+1);


	if( (graphics_buffer = malloc(imagesize(x,y,x+pixel_width,y+pixel_height))) ==
		  NULL )
		allocation_error( "display_menu" );


	getimage( x, y, x+pixel_width, y+pixel_height, graphics_buffer );

	box( 2, x, y, x+pixel_width, y+pixel_height );

	setcolor( BLUE );

	for( loop=0; loop != number_of_options; loop++ )
	{
		position_counter=0;  position_flag=0;

		while( *(options+(loop*characters_width)+position_counter) != '\0' )
		{
			if( *(options+(loop*characters_width)+position_counter) == '@' )
			{
				option_key[loop] = toupper( *(options+(loop*characters_width)+
													 position_counter+1) );
				position_counter++;
				setcolor( RED );

				outtextxy( x+(position_counter*textwidth("T")),
							  y+((loop+1)*textheight("T")),
							  sub_string( options+(loop*characters_width),
											  position_counter+1, 1 ) );

				setcolor( BLUE );
				position_flag = -1;
			}
			else
			{
			outtextxy( x+textwidth("T")+((position_counter+position_flag)*
						  textwidth("T")), y+((loop+1)*textheight("T")),
						  sub_string( options+(loop*characters_width),
										  position_counter+1, 1 ) );
			}
			position_counter++;
		}
	}


	show_mouse();


	do
	{
		do
		{
			inregs.h.al = 0x00;
			inregs.h.ah = 0x06;
			inregs.h.dl = 0xff;
			int86( 0x21, &inregs, &outregs );
			key_test = outregs.h.al;

			if( mouse_installed == YES )
			{
				inregs.x.ax = 0x03;
				int86( 0x33, &inregs, &outregs );
				mouse_test = outregs.x.bx;
			}
		}
		while( key_test < 27 && mouse_test == NO );

		if( mouse_test != NO )
		{
			valid_input_flag=YES;

			mouse_x = outregs.x.cx;
			mouse_y = outregs.x.dx;

			if( (mouse_x > x) && (mouse_x < x+pixel_width) &&
				 (mouse_y > y+textheight("T")+1) &&
				 (mouse_y < y+(textheight("T")*(number_of_options+1)-1) ))
			{
				return_value = (mouse_y-1-y)/textheight("T");
			}
			else
			{
				return_value = -1;
			}

			if( (mouse_x > x) && (mouse_x < x+100*xratio/640) &&
				 (mouse_y < y) )

			valid_input_flag=NO;
		}
		else
		{
			key_test -= 32;

			for( loop=0; loop != number_of_options; loop++ )
			{
				if( option_key[loop] == key_test )
					return_value = loop+1;
			}
			if( (return_value != -1) || (key_test == -5 ) )
				valid_input_flag=YES;

		}
	}
	while( valid_input_flag == NO );


	hide_mouse();


	if( return_value > 0 )
	{
		setfillstyle( SOLID_FILL, LIGHTRED );
		bar( x+2, (y+(textheight("T")*return_value)), x+pixel_width-2,
			  (y+(textheight("T")*(return_value+1))) );
	}

	putimage( x, y, graphics_buffer, COPY_PUT );
	free( graphics_buffer );


	show_mouse();


	return return_value;
}

initialise_mouse()
{
	union REGS inregs, outregs;

	inregs.x.ax = 0;
	int86( 0x33, &inregs, &outregs );

	if( outregs.x.ax == 0 )
		mouse_installed = NO;
}


show_mouse()
{
	union REGS regs;

	regs.x.ax = 0x1;
	int86( 0x33, &regs, &regs );
}


hide_mouse()
{
	union REGS regs;

	regs.x.ax = 0x2;
	int86( 0x33, &regs, &regs );
}

char * sub_string( char *string, int start_position, int number_of_characters )
{
	char *pointer_to_sub_string;


	free( sub_string_store );


	if( (sub_string_store = (char *) malloc(sizeof(char)*strlen(string)+2)) == NULL)
		allocation_error( "sub_string" );


	strcpy( sub_string_store, string );

	pointer_to_sub_string = sub_string_store+start_position-1;

	*(sub_string_store+start_position+number_of_characters-1) = '\0';


	return pointer_to_sub_string;
}

int wait_for_input()
{
	int key_test, mouse_test=NO, mouse_x, mouse_y, menu_number;
	int valid_input_flag=NO;
	union REGS inregs, outregs;

	show_mouse();

	do
	{
		do
		{
			inregs.h.al = 0x00;
			inregs.h.ah = 0x06;
			inregs.h.dl = 0xff;
			int86( 0x21, &inregs, &outregs );
			key_test = outregs.h.al;

			if( mouse_installed == YES )
			{
				inregs.x.ax = 0x03;
				int86( 0x33, &inregs, &outregs );
				mouse_test = outregs.x.bx;
			}
		}
		while( (key_test==0 || key_test>26) && mouse_test == NO );

		if( mouse_test != NO )
		{
			mouse_x = outregs.x.cx;
			mouse_y = outregs.x.dx;

			if( (mouse_y > (10*yratio/480)) &&
				 (mouse_y < (10*yratio/480+textheight("T"))) )
			{
				if( mouse_x < 600*xratio/640 )
				{
					menu_number = mouse_x/(100*xratio/640)+1;
					valid_input_flag = YES;
				}
			}
		}
		else
		{
			valid_input_flag = YES;

			switch( key_test )
			{
				case 6  :  menu_number = 1; break;
				case 7  :  menu_number = 2; break;
				case 16 :  menu_number = 3; break;
				case 15 :  menu_number = 4; break;
				case 26 :  menu_number = 5; break;
				case 17 :  menu_number = 6; break;

				default :  valid_input_flag = NO;
			}
		}
	}
	while( valid_input_flag == NO );

	switch( menu_number )
	{
		case 1 :  FILEmenu(); break;

		case 2 :  GRAPHmenu(); break;

		case 3 :  PRINTmenu(); break;

		case 4 :  OPTIONSmenu(); break;

		case 5 :  ZOOMmenu(); break;

		case 6 :  if( QUITmenu() == YES )
					return NO;
				break;
	}


	return YES;
}

FILEmenu()
{
	char *options;
	int chosen_option;


	if( (options = (char *) malloc(sizeof(char)*50)) == NULL )
		allocation_error( "FILEmenu" );


	strcpy( options,    " @Load Graph" );
	strcpy( options+15, " @Save graph" );
	strcpy( options+30, " Save @as ..." );
	*(options+45) = -1;


	chosen_option = display_menu( 0, (19*yratio/480)+textheight("T"), options, 15 );


	switch( chosen_option )
	{
		case 1: load_graph(); break;

		case 2: save_graph( NO );  break;

		case 3: save_graph( YES ); break;
	}


	free( options );
}

GRAPHmenu()
{
	char *options;
	int chosen_option;
	double new_minimum_x, new_maximum_x, new_minimum_y, new_maximum_y;


	if( (options = (char *) malloc(sizeof(char)*80)) == NULL )
		allocation_error( "GRAPHmenu" );


	strcpy( options,    " New @graph" );
	strcpy( options+15, " New @formula" );
	strcpy( options+30, " New xy-ax@es" );
	strcpy( options+45, " New @x-axis" );
	strcpy( options+60, " New @y-axis" );
	*(options+75) = -1;


	chosen_option = display_menu( 100*xratio/640, (19*yratio/480)+textheight("T"),
						 options, 15 );

	switch( chosen_option )
	{
		case 1 :  prompt_user_for_data( ALL, &new_minimum_x, &new_maximum_x,
												  &new_minimum_y, &new_maximum_y );

					 draw_graph( new_minimum_x, new_maximum_x,
									 new_minimum_y, new_maximum_y );

					 break;

		case 2 :  if( original_formula[0] == '\0' )
					 {
						display_error_message( 50*xratio/640, 150*yratio/480,
						"A graph has not been defined. Select 'New graph' option",
						"continue" );
						break;
					 }

					 prompt_user_for_data( FORMULA, NULL, NULL, NULL, NULL );

					 draw_graph( minimum_x, (minimum_x+5*division_x),
									 minimum_y, (minimum_y+5*division_y) );
					 break;

		case 3 :  if( original_formula[0] == '\0' )
					 {
						display_error_message( 50*xratio/640, 150*yratio/480,
						"A graph has not been defined. Select 'New graph' option",
						"continue" );
						break;
					 }

					 prompt_user_for_data( XYAXES, &new_minimum_x, &new_maximum_x,
												  &new_minimum_y, &new_maximum_y );

					 draw_graph( new_minimum_x, new_maximum_x,
									 new_minimum_y, new_maximum_y );

					 break;

		case 4 :  if( original_formula[0] == '\0' )
					 {
						display_error_message( 50*xratio/640, 150*yratio/480,
						"A graph has not been defined. Select 'New graph' option",
						"continue" );
						break;
					 }

					 prompt_user_for_data( XAXIS, &new_minimum_x, &new_maximum_x,
												  NULL, NULL );

					 draw_graph( new_minimum_x, new_maximum_x,
									 minimum_y, (minimum_y+5*division_y) );

					 break;

		case 5 :  if( original_formula[0] == '\0' )
					 {
						display_error_message( 50*xratio/640, 150*yratio/480,
							"A graph has not been defined. Select 'New graph' option",
							"continue" );
							break;
					 }

					 prompt_user_for_data( YAXIS, NULL, NULL, &new_minimum_y,
												  &new_maximum_y );

					 draw_graph( minimum_x, minimum_x+5*division_x,
									 new_minimum_y, new_maximum_y );

					 break;
	}


	free( options );
}

PRINTmenu()
{
	char *options;
	int chosen_option;


	if( (options = (char *) malloc(sizeof(char)*50)) == NULL )
		allocation_error( "PRINTmenu" );


	strcpy( options, " @Print - 24-pin" );
	strcpy( options+20, " P@rint - 9-pin" );
	*(options+40) = -1;


	chosen_option = display_menu( 200*xratio/640, (19*yratio/480)+textheight("T"),
											options, 20 );


	switch( chosen_option )
	{
		case 1 : print_graph( 24 );  break;

		case 2 : print_graph( 9 );   break;
	}


	free( options );
}

OPTIONSmenu()
{
	char *options;
	int chosen_option;


	if( (options = (char *) malloc(sizeof(char)*70)) == NULL )
		allocation_error( "OPTIONSmenu" );


	strcpy( options, " @Speed" );

	if( radians_flag == YES )
		strcpy( options+15, " @Degrees" );
	else
		strcpy( options+15, " @Radians" );

	if( grid_flag == YES )
		strcpy( options+30, " @Grid off" );
	else
		strcpy( options+30, " @Grid on" );

	strcpy( options+45, " Redra@w" );

	*(options+60) = -1;


	chosen_option = display_menu( 300*xratio/640, (19*yratio/480)+textheight("T"),
						options, 15 );


	switch( chosen_option )
	{

		case 1 :  data_saved_flag=NO;
					 Change_speed();
					 break;

		case 2 :  data_saved_flag=NO;
					 if( radians_flag == YES )
						radians_flag = NO;
					 else
						radians_flag = YES;
					 break;

		case 3 :  data_saved_flag=NO;
					 if( grid_flag == YES )
						grid_flag = NO;
					 else
						grid_flag = YES;

					 break;

		case 4 :  if( original_formula[0] == '\0' )
					 {
						display_error_message( 50*xratio/640, 130*yratio/480,
						  "A graph has not yet been defined. Select 'New graph' option",
						  "continue" );
						break;
					 }

					 draw_graph( minimum_x, minimum_x+5*division_x, minimum_y,
									 minimum_y+5*division_y );

					 break;
	}


	free( options );
}

ZOOMmenu()
{
	char *options;
	int chosen_option;


	if( mouse_installed == NO )
	{
		display_error_message( 80*xratio/640, 130*yratio/480,
			"The zoom function cannot be used without a mouse", "continue" );
		return;
	}


	if( (options = malloc(sizeof(char)*50)) == NULL )
		allocation_error( "ZOOMmenu" );


	strcpy( options, " Zoom @in" );
	strcpy( options+15, " Zoom @out" );
	strcpy( options+30, " @Clear" );
	*(options+45) = -1;


	chosen_option = display_menu( 400*xratio/640, (19*yratio/480)+textheight("T"),
											options, 15 );


	switch( chosen_option )
	{
		case 1 : zoom_in();
					break;

		case 2 : zoom_out();
					break;

		case 3 : coordinate_stack_pointer = 0;
					break;
	}


	free( options );
}

int QUITmenu()
{
	char *options;
	int quit_flag, chosen_option;


	if( (options = (char *)malloc(sizeof(char)*40)) == NULL )
		allocation_error( "QUITmenu" );


	strcpy( options,    " Sure - @Yes" );
	strcpy( options+15, "        @No" );
	*(options+30) = -1;


	chosen_option = display_menu( 500*xratio/640, (19*yratio/480)+textheight("T"),
											options, 15 );


	if( chosen_option == 1 )
	{
		if( data_saved_flag == NO )
		{
			if( display_warning_message( 130*xratio/640, 130*yratio/480,
				"The current data has not been saved.",
				"Do you still wish to quit (Y/N) ?" ) == YES )

				quit_flag = YES;
			else
				quit_flag = NO;

		}
		else
			quit_flag = YES;
	}
	else
		quit_flag = NO;


	free( options );


	return quit_flag;
}




prompt_user_for_data( int data_required, double *new_minimum_x,
					  double *new_maximum_x, double *new_minimum_y,
					  double *new_maximum_y )
{
	int x1, x2, formula_y1, x_axis_y1, y_axis_y1, y2;
	int valid_input_flag=YES;
	char new_xmin_string[20], new_xmax_string[20], new_ymin_string[20];
	char new_ymax_string[20];
	char *formula_graphics_buffer, *x_axis_graphics_buffer;
	char *y_axis_graphics_buffer;
	char error_message[100];
	unsigned char formula_store[150];
	double storage_area;


	coordinate_stack_pointer = 0;
	data_saved_flag = NO;


	x1 = 20*xratio/640;
	x2 = 620*xratio/640;


	hide_mouse();


	if( data_required == FORMULA || data_required == ALL )
	{
		formula_y1 = 50*yratio/480;
		y2 = 70*yratio/480+textheight("T");


		if( (formula_graphics_buffer = malloc(imagesize(x1,formula_y1,x2,y2))) ==
			  NULL )
			allocation_error( "prompt_user_for_data" );


		getimage( x1, formula_y1, x2, y2, formula_graphics_buffer );


		do
		{
			box( 2, x1, formula_y1, x2, y2 );
			text_input( "Formula : ", x1+(10*xratio/640), formula_y1+(12*yratio/480),
					50, original_formula );


			tidy_the_formula();
			valid_input_flag = test_the_formula( error_message );


			if( valid_input_flag == NO )
			{
				display_error_message( x1, y2+10*yratio/480, error_message,
					"retry" );
				continue;
			}


			strcpy( formula_store, original_formula );

			valid_input_flag = translate_formula( formula_store, error_message );

			if( valid_input_flag == NO )
				display_error_message( x1, y2+10*yratio/480, error_message,
					"retry" );
		}
		while( valid_input_flag == NO );

	}


	if( data_required == XYAXES || data_required == XAXIS || data_required == ALL)
	{
		x_axis_y1 = 100*yratio/480;
		y2 = 120*yratio/480+(2*textheight("T"));


		if( (x_axis_graphics_buffer = malloc( imagesize(x1,x_axis_y1,x2,y2))) ==
			  NULL )
			allocation_error( "prompt_user_for_data" );


		getimage( x1, x_axis_y1, x2, y2, x_axis_graphics_buffer );


		do
		{
			box( 2, x1, x_axis_y1, x2, y2 );

			text_input( "Enter the minimum x-coordinate : ", x1+10*xratio/640,
					x_axis_y1+(12*yratio/480), 12, new_xmin_string );
			text_input( "Enter the maximum x-coordinate : ", x1+10*xratio/640,
					x_axis_y1+(12*yratio/480)+textheight("T"), 12,
					new_xmax_string );


			valid_input_flag = check_for_digits( new_xmin_string );


			if( valid_input_flag == YES )
				valid_input_flag = check_for_digits( new_xmax_string );

			if( valid_input_flag == YES )
			{
				*new_minimum_x = atof( new_xmin_string );
				*new_maximum_x = atof( new_xmax_string );

				if( *new_minimum_x == *new_maximum_x )
				{
					valid_input_flag = NO;
					display_error_message( x1, y2+10*yratio/480,
						"The minimum and maximum x-values cannot be equal", "retry" );
				}
				else
				{
					if( *new_minimum_x > *new_maximum_x )
					{
						storage_area = *new_minimum_x;
						*new_minimum_x = *new_maximum_x;
						*new_maximum_x = storage_area;
					}


					if( *new_minimum_x < -99999999 || *new_maximum_x > 99999999 )
					{
						valid_input_flag = NO;
						display_error_message( x1, y2+10*yratio/480,
							"Please use values in the range +/-99,999,999", "retry" );
					}
					else
						if( check_for_accuracy( *new_minimum_x, *new_maximum_x ) == NO)
						{
							valid_input_flag = NO;
							display_error_message( x1, y2+10*yratio/480,
								"The graph cannot be drawn to this accuracy.", "retry" );
						}
					}
			}
			else
				display_error_message( x1, y2+10*yratio/480,
					"One of the x-coordinates was not a valid number", "retry" );
		}
		while( valid_input_flag == NO );

	}

	if( data_required == XYAXES || data_required == YAXIS || data_required == ALL)
	{
		y_axis_y1 = 160*yratio/480;
		y2 = 180*yratio/480+(2*textheight("T"));


		if( (y_axis_graphics_buffer = malloc( imagesize(x1,y_axis_y1,x2,y2))) == NULL )
			allocation_error( "prompt_user_for_data" );


		getimage( x1, y_axis_y1, x2, y2, y_axis_graphics_buffer );


		do
		{
			box( 2, x1, y_axis_y1, x2, y2 );

			text_input( "Enter the minimum y-coordinate : ", x1+10*xratio/640,
				y_axis_y1+(12*yratio/480), 12, new_ymin_string );
			text_input( "Enter the maximum y-coordinate : ", x1+10*xratio/640,
				y_axis_y1+(12*yratio/480)+textheight("T"), 12,
				new_ymax_string );


			valid_input_flag = check_for_digits( new_ymin_string );


			if( valid_input_flag == YES )
				valid_input_flag = check_for_digits( new_ymax_string );

			if( valid_input_flag == YES )
			{
				*new_minimum_y = atof( new_ymin_string );
				*new_maximum_y = atof( new_ymax_string );

				if( *new_minimum_y == *new_maximum_y )
				{
					valid_input_flag = NO;
					display_error_message( x1, y2+10*yratio/480,
						"The minimum and maximum y-values cannot be equal", "retry" );
				}
				else
				{
					if( *new_minimum_y > *new_maximum_y )
					{
						storage_area = *new_minimum_y;
						*new_minimum_y = *new_maximum_y;
						*new_maximum_y = storage_area;
					}

					if( *new_minimum_y < -99999999 || *new_maximum_y > 99999999 )
					{
						valid_input_flag = NO;
						display_error_message( x1, y2+10*yratio/480,
							"Please use values in the range +/- 99,999,999", "retry" );
					}
					else
						if( check_for_accuracy( *new_minimum_y, *new_maximum_y ) == NO)
						{
							valid_input_flag = NO;
							display_error_message( x1, y2+10*yratio/480,
								"The graph cannot be drawn to this accuracy.", "retry" );
						}
				}
			}
			else
				display_error_message( x1, y2+10*yratio/480,
					"One of the y-coordinates was not a valid number", "retry" );


		}
		while( valid_input_flag == NO );

	}


	if( data_required == ALL || data_required == FORMULA )
	{
		putimage( x1, formula_y1, formula_graphics_buffer, COPY_PUT );
		free( formula_graphics_buffer );
	}


	if( data_required == ALL || data_required == XYAXES || data_required == XAXIS)
	{
		putimage( x1, x_axis_y1, x_axis_graphics_buffer, COPY_PUT );
		free( x_axis_graphics_buffer );
	}


	if( data_required == ALL || data_required == XYAXES || data_required == YAXIS)
	{
		putimage( x1, y_axis_y1, y_axis_graphics_buffer, COPY_PUT );
		free( y_axis_graphics_buffer );
	}
}


Change_speed()
{
	int x1, y1, x2, y2, valid_input_flag;
	char *graphics_buffer, speed_string[3];


	hide_mouse();


	x1 = 50*xratio/640;
	x2 = 350*xratio/640;
	y1 = 90*yratio/480;
	y2 = (110*yratio/480)+textheight("T");


	if( (graphics_buffer=malloc(imagesize(x1,y1,x2,y2)))==NULL )
		allocation_error( "Change_speed" );


	getimage( x1, y1, x2, y2, graphics_buffer );


	do
	{
		valid_input_flag = YES;

		box( 2, x1, y1, x2, y2 );
		text_input( "New speed (1-10) : ", x1+(10*xratio/640),
						y1+(12*yratio/480), 2, speed_string );


		speed = atoi( speed_string );


		if( speed == 0 || speed > 10 || speed < 1 )
		{
			valid_input_flag = NO;
			display_error_message( x1, y2+20*yratio/480,
				"Invalid speed. Use a value from 1 to 10.", "retry" );
		}
	}
	while( valid_input_flag == NO );


	putimage( x1, y1, graphics_buffer, COPY_PUT );

	free( graphics_buffer );
}

draw_graph( double new_minimum_x, double new_maximum_x,
			double new_minimum_y, double new_maximum_y )
{
	char label_store[15], text_store[100];
	float left, right, top, bottom, label_position;
	int loop, division_exponent, decimal_places;


	hide_mouse();


	left   = textwidth( "-0.000000000" )+(10*xratio/640);
	right  = xratio-1;
	top    = (19*yratio/480)+textheight("T");
	bottom = (450*yratio/480)-(2*textheight("T"));


	setfillstyle( SOLID_FILL, LIGHTCYAN );
	bar( 0, (19*yratio/480)+textheight("T"), xratio-1, yratio-1 );


	setcolor( BLACK );
	rectangle( 0, 18*yratio/480+textheight("T"), xratio-1, yratio-1 );


	moveto( left, top );
	lineto( left, bottom );


	moveto( left, bottom+14*yratio/480+textheight("T") );
	lineto( left, yratio-1 );


	moveto( left, bottom );
	lineto( right, bottom );


	minimum_y = new_minimum_y;
	division_y = (new_maximum_y-new_minimum_y)/5;
	sprintf( text_store, "%.9e", division_y );
	division_exponent = atoi( text_store+11 );


	decimal_places = abs( division_exponent )+1;
	if( division_exponent > 0 )
		decimal_places = 0;


	for( loop=0; loop <= 4; loop++ )
	{
		label_position = ((bottom-top)/5)*(5-loop)+top;


		moveto( left, label_position );
		lineto( left-(3*xratio/640), label_position );

		convert_double_to_string( minimum_y+(division_y*loop), label_store,
								  decimal_places );


		if( loop == 0 )
		{
			moveto( 0, bottom );
			lineto( left-10-textwidth(label_store), bottom );
		}
		else
		{
			if( grid_flag == YES )
			{
				moveto( left, label_position );
				setlinestyle( USERBIT_LINE, 0x1111, 1 );
				lineto( right, label_position );
				setlinestyle( SOLID_LINE, 0, 1 );
			}
		}


		outtextxy( left-5-textwidth(label_store),
					  label_position-(textheight("T")/2)+(1*yratio/480),
					  label_store );
	}


	minimum_x = new_minimum_x;
	division_x = (new_maximum_x-new_minimum_x)/5;


	sprintf( text_store, "%.9e", division_x );
	division_exponent = atoi( text_store+11 );


	decimal_places = abs( division_exponent )+1;
	if( division_exponent > 0 )
		decimal_places = 0;


	for( loop=0; loop <= 4; loop++ )
	{
		label_position = ((right-left)/5)*loop+left;


		moveto( label_position, bottom );
		lineto( label_position, bottom+(3*yratio/480) );


		if( loop != 0 && grid_flag == YES )
		{
			moveto( label_position, bottom );
			setlinestyle( USERBIT_LINE, 0x1111, 1 );
			lineto( label_position, top );
			setlinestyle( SOLID_LINE, 0, 1 );
		}


		convert_double_to_string( minimum_x+(division_x*loop), label_store,
								  decimal_places );


		outtextxy( label_position-(textwidth(label_store)/2),
					  bottom+(10*yratio/480), label_store );
	}


	sprintf( text_store, "Speed %d", speed );
	box( 1, 4*xratio/640, top+6*yratio/480, left-4*xratio/640,
		   top+17*yratio/480+textheight("T") );
	setcolor( BLUE );
	outtextxy( 14*xratio/640, top+12*yratio/480, text_store );


	box( 1, 4*xratio/640, top+24*yratio/480+textheight("T"),
		   left-4*xratio/640, top+35*yratio/480+(2*textheight("T")) );


	if( radians_flag == NO )
		outtextxy( 14*xratio/640, top+30*yratio/480+textheight("T"),
					  "Degrees" );
	else
		outtextxy( 14*xratio/640, top+30*yratio/480+textheight("T"),
					  "Radians" );


	strcpy( text_store, "y=" );
	strcat( text_store, original_formula );
	box( 1, left+20*xratio/640, 454*yratio/480,
			  left+(60*xratio/640)+textwidth( text_store ),
			  474*yratio/480 );
	outtextxy( left+40*xratio/640, 460*yratio/480, text_store );


	box( 1, 4*xratio/640, top+195*yratio/480,
			14*xratio/640+textwidth("Y"), top+199*yratio/480+textheight("T") );
	outtextxy( 9*xratio/640, top+198*yratio/480, "Y" );


	box( 1, right-15*xratio/640-textwidth("X"), 457*yratio/480, right-6,
			461*yratio/480+textheight("T") );
	outtextxy( right-10*xratio/640-textwidth("X"), 460*yratio/480, "X" );


	box( 1, 4*xratio/640, 454*yratio/480, left-4*xratio/640,
			474*yratio/480 );
	outtextxy( 14*xratio/640, 460*yratio/480, "Drawing..." );


	plot_points( left, right, top, bottom );


	setfillstyle( SOLID_FILL, LIGHTCYAN );
	bar( 4*xratio/640, 452*yratio/480, left-4*xratio/640, 476*yratio/480 );


	show_mouse();
}

plot_points( int left, int right, int top, int bottom )
{
	double x, y1, y2, x_constant, y_constant;
	double point_one, point_two;
	int continue_flag;


	setcolor( MAGENTA );
	setviewport( left, top, right, bottom, 1 );


	x_constant = 5*division_x/(right-left);
	y_constant = (bottom-top)/(5*division_y);


	point_two = calculate_point( minimum_x );


	for( x=speed; x < right-left; x+=speed )
	{
		point_one = point_two;
		point_two = calculate_point( minimum_x+x_constant*x );


		if( point_defined_flag == NO )
		{
			continue_flag = point_not_defined_error();

			if( continue_flag == YES )
			{
				x += speed;
				point_defined_flag = YES;

				point_two = calculate_point( minimum_x+x_constant*x );

				continue;
			}

			else
			{
				point_defined_flag = YES;
				break;
			}
		}


		y1 = (minimum_y+5*division_y-point_one)*y_constant;
		y2 = (minimum_y+5*division_y-point_two)*y_constant;


		if( y1 < -32000 )
			y1 = -32000;

		if( y1 > 32000 )
			y1 = 32000;

		if( y2 < -32000 )
			y2 = -32000;

		if( y2 > 32000 )
			y2 = 32000;


		if( point_defined_flag == YES )
		{
			moveto( x-speed, y1 );
			lineto( x, y2 );
		}
	}


	setviewport( 0, 0, xratio-1, yratio-1, 0 );
}


text_input( char *prompt, int x, int y, int maximum_characters, char *reply )
{
	int counter=0, terminate_flag = NO;


	setcolor( BLUE );
	outtextxy( x, y, prompt );


	x += textwidth( prompt );


	do
	{
		*(reply+counter+1) = '\0';
		*(reply+counter) = getch();


		if( *(reply+counter) == 13 )
			terminate_flag = YES;
		else
		{
			if( (*(reply+counter) == 8) && (counter > 0 ) )
			{
				counter--;

				setcolor( WHITE );
				outtextxy( x+(counter*textwidth("m")), y, (reply+counter) );
			}
			else
			{
				if( *(reply+counter) != 8 )
				{
					if( counter < maximum_characters )
					{
						setcolor( BLUE );
						outtextxy( x+(counter*textwidth("m")), y, (reply+counter) );

						counter++;
					}
				}
			}
		}

	}
	while( terminate_flag == NO );


	*(reply+counter) = '\0';
}

convert_double_to_string( double number, char *string, int decimal_places )
{
	char format_string[5];


	if( decimal_places < 0 )
		decimal_places = 0;


	if( decimal_places > 9 )
		decimal_places = 9;


	strcpy( format_string, "%. f" );

	format_string[2] = 48+decimal_places;

	sprintf( string, format_string, number );
}

double calculate_point( double x )
{
	int pointer=0;
	double real_left, real_right;


	do
	{
		if( f_table[pointer].leftvalue < 91 )
			real_left = number_table[f_table[pointer].leftvalue-65];
		else
			if( f_table[pointer].leftvalue == 120 )
				real_left = x;
			else
				if( f_table[pointer].leftvalue == 620 )
					real_left = -1*x;
				else
					if( f_table[pointer].leftvalue < 600 )
						real_left = -1*number_table[f_table[pointer].leftvalue-565];
					else
						if( f_table[pointer].leftvalue < 1500 )
							real_left = f_t_values[f_table[pointer].leftvalue-1000];
						else
							real_left = -1*f_t_values[f_table[pointer].leftvalue-1500];



		if( f_table[pointer].rightvalue < 91 )
			real_right = number_table[f_table[pointer].rightvalue-65];
		else
			if( f_table[pointer].rightvalue == 120 )
				real_right = x;
			else
				if( f_table[pointer].rightvalue == 620 )
					real_right = -1*x;
				else
					if( f_table[pointer].rightvalue < 600 )
						real_right = -1*number_table[f_table[pointer].rightvalue-565];
					else
						if( f_table[pointer].rightvalue < 1500 )
							real_right = f_t_values[f_table[pointer].rightvalue-1000];
						else
							real_right = -1*f_t_values[f_table[pointer].rightvalue-1500];



		switch( f_table[pointer].operator )
		{

			case 94 :  f_t_values[pointer] =
						  power( real_left, real_right );
						  break;

			case 42 :  f_t_values[pointer] =
						  real_left*real_right;
						  break;

			case 47 :  f_t_values[pointer] =
						  divide( real_left, real_right );
						  break;

			case 43 :  f_t_values[pointer] =
						  real_left+real_right;
						  break;

			case 45 :  f_t_values[pointer] =
						  real_left-real_right;
						  break;

			case 128 : f_t_values[pointer] =
						  sine( real_left );
						  break;

			case 129 : f_t_values[pointer] =
						  cosine( real_left );
						  break;

			case 130 : f_t_values[pointer] =
						  tangent( real_left );
						  break;

			case 131 : f_t_values[pointer] =
						  arcsin( real_left );
						  break;

			case 132 : f_t_values[pointer] =
						  arccos( real_left );
						  break;

			case 133 : f_t_values[pointer] =
						  arctan( real_left );
						  break;

			case 134 : f_t_values[pointer] =
						  sinh( real_left );
						  break;

			case 135 : f_t_values[pointer] =
						  cosh( real_left );
						  break;

			case 136 : f_t_values[pointer] =
						  tanh( real_left );
						  break;

			case 137 : f_t_values[pointer] =
						  fabs( real_left );
						  break;

			case 138 : f_t_values[pointer] =
						  logbase10( real_left );
						  break;

			case 139 : f_t_values[pointer] =
						  logbaseE( real_left );
						  break;

			case 140 : f_t_values[pointer] =
						  exp( real_left );
						  break;

			case 141 : f_t_values[pointer] =
						  squareroot( real_left );
						  break;

			case 0   : f_t_values[pointer] =
						  real_left;
						  break;

			case 255 : return real_left;
		}

		pointer++;
	}
	while( 1 );
}

int translate_formula( unsigned char *formula, char *error_message )
{
	int formula_array[150], error_flag, loop;


	error_flag = abbreviate_functions( formula );


	if( error_flag == NO )
	{
		strcpy( error_message, "There is an unknown function in the formula" );
		return NO;
	}


	change_numbers_to_letters( formula );

	add_multiply_operator( formula );


	formula_array[0] = strlen( formula );
	for( loop=1; loop <= strlen(formula); loop++ )
		formula_array[loop] = formula[loop-1];


	error_flag = create_formula_table( formula_array );


	if( error_flag == ERROR )
	{
		strcpy( error_message, "There is an error in the syntax of the formula" );
		return NO;
	}
}


int abbreviate_functions( unsigned char *formula )
{
	int found_function_flag, x, y, z, loop;
	unsigned char formula_store[150];


	for( x = 0; x < strlen(formula); x++ )
	{
		for( y=0; y <= FUNCTIONS; y++ )
		{
			if( function_table[y][0] == formula[x] )
			{

				for( z=1; z <= strlen(formula)-x; z++ )
				{

					if( z > 5 )
						break;

					if( function_table[y][z] == formula[x+z] )
						found_function_flag = YES;
					else
					{
						if( function_table[y][z] == '$' )
						{
							if( formula[x+z] != 'x' && islower(formula[x+z]) )
							{
								found_function_flag = NO;
								break;
							}
							else
							{
								found_function_flag = YES;
								break;
							}
						}
						else
						{
							found_function_flag = NO;
							break;
						}
					}
				}
			}

			if( found_function_flag == YES )
			{
				strcpy( formula_store, sub_string( formula, 1, x ) );
				formula_store[x] = y+128;
				formula_store[x+1] = '\0';
				strcat( formula_store, sub_string( formula, x+z+1,
						  strlen(formula)-(x+z) ));
				strcpy( formula, formula_store );

				found_function_flag = NO; break;
			}

		}
	}


	for( loop=0; loop != strlen( formula ); loop++ )
		if( (islower(formula[loop])) && (formula[loop] != 'x') )
			return NO;


	return YES;
}


change_numbers_to_letters( unsigned char *formula )
{
	int counter=0, loop, number_table_pointer=0;
	char *dummy, letter[2];
	unsigned char formula_store[150];


	do
	{
		if( formula[counter] >= '0' && formula[counter] <= '9' )
		{

			number_table[number_table_pointer] = strtod( formula+counter, &dummy );


			for( loop = counter+1; loop <= strlen( formula ); loop++ )
			{

				if( (formula[loop] < '0' || formula[loop] > '9') &&
					 formula[loop] != '.' )
				{

					strcpy( formula_store, sub_string( formula, 1, counter ) );

					letter[0] = 65+number_table_pointer;
					letter[1] = '\0';

					strcat( formula_store, letter );
					strcat( formula_store, sub_string( formula, loop+1,
									strlen(formula)-loop ) );

					strcpy( formula, formula_store );
					break;

				}
			}

			number_table_pointer++;
		}

		counter++;
	}
	while( counter <= strlen( formula ) );

}


int create_formula_table( int *formula_array )
{
	int sub_section[150];
	int brackets_flag, operators_flag, finished_flag=NO;
	f_t_pointer = 0;


	do
	{
		brackets_flag = pick_out_sub_section( formula_array, sub_section );


		if( brackets_flag == YES && sub_section[0] == 0 )
			return ERROR;


		if( brackets_flag == YES )
		{
			operators_flag = translate_sub_section( sub_section );

			if( operators_flag == ERROR )
				return ERROR;

			if( operators_flag == NO )
			{
				f_table[f_t_pointer].operator = 0;

				if( sub_section[1] != 45 )
					f_table[f_t_pointer].leftvalue = sub_section[1];
				else
					f_table[f_t_pointer].leftvalue = sub_section[2]+500;

				sub_section[0] = 1;
				sub_section[1] = f_t_pointer+1000;
				f_t_pointer++;
			}

			amend_the_formula( formula_array, sub_section );
		}
		else
		{
			operators_flag = translate_sub_section( formula_array );

			if( operators_flag == ERROR )
				return ERROR;

			finished_flag = YES;
		}
	}
	while( finished_flag == NO );


	f_table[f_t_pointer].operator = 255;

	if( formula_array[1] == 45 )
		f_table[f_t_pointer].leftvalue = formula_array[2]+500;
	else
		f_table[f_t_pointer].leftvalue = formula_array[1];


	return YES;
}


amend_the_formula( int *formula_array, int *sub_section )
{
	int loop;
	unsigned int formula_store[150];


	int start = formula_array[formula_array[0]+1];
	int end   = formula_array[formula_array[0]+2];


	if( sub_section[0] == 2 && sub_section[1] == 45 )
	{
		sub_section[0] = 1;
		sub_section[1] = sub_section[2]+500;
	}


	for( loop=1; loop < start; loop++ )
		formula_store[loop] = formula_array[loop];


	formula_store[start] = sub_section[1];


	for( loop=end+1; loop < formula_array[0]+1; loop++ )
		formula_store[loop-(end-start)] = formula_array[loop];


	formula_store[0] = formula_array[0]-(end-start);


	for( loop=0; loop <= formula_store[0]; loop++ )
		formula_array[loop] = formula_store[loop];
}


int translate_sub_section( int *sub_section )
{
	int found_expression_flag[4];


	found_expression_flag[0] = translate_functions( sub_section );


	found_expression_flag[1] = translate_operators( sub_section, '^', '^' );
	found_expression_flag[2] = translate_operators( sub_section, '*', '/' );
	found_expression_flag[3] = translate_operators( sub_section, '+', '-' );


	if( found_expression_flag[0] == ERROR || found_expression_flag[1] == ERROR ||
		found_expression_flag[2] == ERROR || found_expression_flag[3] == ERROR )

		return ERROR;



	if( found_expression_flag[0] == NO && found_expression_flag[1] == NO &&
		found_expression_flag[2] == NO && found_expression_flag[3] == NO )

		return NO;
	else
		return YES;
}

int translate_operators( int *sub_section, int operator_one, int operator_two )
{
	int loop_one, loop_two, exit_loop_flag=0, minus_flag_one=0;
	int minus_flag_two=0, found_operator_flag = NO;


	do
	{
		for( loop_one=1; loop_one <= sub_section[0]; loop_one++ )
		{
			exit_loop_flag = NO;


			if( sub_section[loop_one] == operator_one ||
				sub_section[loop_one] == operator_two )
			{
				if( operator_two == 45 && loop_one == 1 )
					if( sub_section[0] == 1 )
						return ERROR;
					else
						exit_loop_flag = NO;


				else
				{
					if( sub_section[0] == loop_one )
						return ERROR;


					if( (sub_section[loop_one-1] < 1000) &&
						(sub_section[loop_one-1] != 120) &&
						(sub_section[loop_one-1]<65 || sub_section[loop_one-1]>90 ) )

						return ERROR;


					if( sub_section[loop_one+1] != 45 )
						if( (sub_section[loop_one+1] < 1000) &&
							(sub_section[loop_one+1] != 120) &&
							(sub_section[loop_one+1]<65 || sub_section[loop_one+1]>90 ) )

							return ERROR;


					if( sub_section[loop_one+1] == 45 )
						if( (sub_section[loop_one+2] < 1000) &&
							(sub_section[loop_one+2] != 120) &&
							(sub_section[loop_one+2]<65 || sub_section[loop_one+2]>90 ) )

							return ERROR;


					found_operator_flag = YES;


					f_table[f_t_pointer].operator = sub_section[loop_one];


					if( sub_section[loop_one-2] != 45 )
						f_table[f_t_pointer].leftvalue = sub_section[loop_one-1];
					else
						if( loop_one == 3 )
							f_table[f_t_pointer].leftvalue = sub_section[loop_one-1]+500;
						else
							f_table[f_t_pointer].leftvalue = sub_section[loop_one-1];


					if( sub_section[loop_one+1] != 45 )
						f_table[f_t_pointer].rightvalue = sub_section[loop_one+1];
					else
						f_table[f_t_pointer].rightvalue = sub_section[loop_one+2]+500;


					exit_loop_flag = YES;
				}
			}
			if( exit_loop_flag == YES )
				break;
		}


		if( exit_loop_flag == NO )
		{
			if( found_operator_flag == YES )
				return YES;
			else
				return NO;
		}


		if( ((f_table[f_t_pointer].leftvalue > 500) &&
			 (f_table[f_t_pointer].leftvalue < 999 )) ||
		    (f_table[f_t_pointer].leftvalue > 1500) )
			minus_flag_one = 1;


		if( ((f_table[f_t_pointer].rightvalue > 500 ) &&
			 (f_table[f_t_pointer].rightvalue < 999 )) ||
		    (f_table[f_t_pointer].rightvalue > 1500) )
			minus_flag_two = 1;


		for( loop_two=loop_one-minus_flag_one;
			  loop_two <= sub_section[0]-(2+minus_flag_one+minus_flag_two);
			  loop_two++ )
			sub_section[loop_two] = sub_section[loop_two+2+minus_flag_one+minus_flag_two];


		sub_section[loop_one-1-minus_flag_one] = f_t_pointer+1000;
		sub_section[0] -= 2+minus_flag_one+minus_flag_two;


		f_t_pointer++;
		minus_flag_one = 0;
		minus_flag_two = 0;

	}
	while( 1 );
}



int translate_functions( int *sub_section )
{
	int loop_one, loop_two, exit_loop_flag=0,
			 found_function_flag = NO;

	do
	{
		exit_loop_flag = NO;


		for( loop_one=1; loop_one <= sub_section[0]; loop_one++ )
		{
			if( sub_section[loop_one] >= 128 && sub_section[loop_one] <= 255 )
			{
				if( loop_one+1 > sub_section[0] )
					return ERROR;


				if( (sub_section[loop_one+1] < 1000 ) &&
					(sub_section[loop_one+1] != 120 ) &&
					(sub_section[loop_one+1]<65 || sub_section[loop_one+1]>90 ) )

					return ERROR;


				found_function_flag = YES;
				f_table[f_t_pointer].operator = sub_section[loop_one];
				f_table[f_t_pointer].leftvalue = sub_section[loop_one+1];
				exit_loop_flag=YES;
			}

			if( exit_loop_flag == YES )
				break;
		}


		if( exit_loop_flag == NO )
		{
			if( found_function_flag == YES )
				return YES;
			else
				return NO;
		}


		for( loop_two=loop_one+1; loop_two <= sub_section[0]; loop_two++ )
		{
			sub_section[loop_two-1] = sub_section[loop_two];
		}


		sub_section[loop_one] = f_t_pointer+1000;
		sub_section[0] -= 1;
		f_t_pointer++;

	}
	while( 1 );
}


int pick_out_sub_section( int *formula_array,
					 int *sub_section )
{
	int outer_loop, inner_loop;


	for( outer_loop=formula_array[0]; outer_loop > 0; outer_loop-- )
	{
		if( formula_array[outer_loop] == 40 )
		{
			for( inner_loop=outer_loop+1; inner_loop <= formula_array[0]; inner_loop++ )
			{
				if( formula_array[inner_loop] == 41 )
				{
					sub_section[0] = inner_loop-outer_loop-1;

					formula_array[formula_array[0]+1] = outer_loop;
					formula_array[formula_array[0]+2] = inner_loop;

					return YES;
				}
				else
					sub_section[inner_loop-outer_loop] = formula_array[inner_loop];
			}
		}
	}


	return NO;
}


double divide( double x, double y )
{
	if( y == 0 )
	{
		point_defined_flag = NO;
		return 0;
	}
	else
		return x/y;
}


double power( double x, double y )
{
	double dummy;

	if( ( (x==0.0) && (y<=0.0) )  ||
		( (x<=0.0) && (modf( y, &dummy ) != 0.0) ) )
	{
		point_defined_flag = NO;
		return 0;
	}
	else
		return pow( x, y );
}


double sine( double x )
{
	if( radians_flag == YES )
		return sin( x );
	else
		return sin( x/180*PI );
}

double cosine( double x )
{
	if( radians_flag == YES )
		return cos( x );
	else
		return cos( x/180*PI );
}

double tangent( double x )
{
	double dummy;

	if( radians_flag == YES )
		return tan( x );
	else
		if( modf( (x+90)/180, &dummy ) == 0.0 )
		{
			point_defined_flag = NO;
			return 0;
		}
		else
			return tan( x/180*PI );
}

double arcsin( double x )
{
	if( fabs(x) > 1 )
	{
		point_defined_flag = NO;
		return 0;
	}
	else
		if( radians_flag == YES )
			return asin( x );
		else
			return asin( x )*180/PI;
}

double arccos( double x )
{
	if( fabs(x) > 1 )
	{
		point_defined_flag = NO;
		return 0;
	}
	else
		if( radians_flag == YES )
			return acos( x );
		else
			return acos( x )*180/PI;
}

double arctan( double x )
{
	if( radians_flag == YES )
		return atan( x );
	else
		return atan( x )*180/PI;
}

double logbase10( double x )
{
	if( x <= 0 )
	{
		point_defined_flag = NO;
		return 0;
	}
	else
		return log10( x );
}

double logbaseE( double x )
{
	if( x <= 0 )
	{
		point_defined_flag = NO;
		return 0;
	}
	else
		return log( x );
}

double squareroot( double x )
{
	if( x < 0 )
	{
		point_defined_flag = NO;
		return 0;
	}
	else
		return sqrt( x );

}



int matherr( struct exception *a )
{
	int x1, y1, x2, y2;
	char *graphics_buffer, error_message[60];


	setviewport( 0,0, xratio-1, yratio-1, 0 );


	if( a->type == DOMAIN )
		strcpy( error_message, "A DOMAIN error has occurred, e.g. log(-1)." );

	if( a->type == SING )
		strcpy( error_message, "A SINGULARITY error has occurred, e.g. 0^-2. " );

	if( a->type == OVERFLOW )
		strcpy( error_message, "An OVERFLOW error has occurred, e.g. 2^10000. " );

	if( a->type == UNDERFLOW )
		strcpy( error_message, "An UNDERFLOW error has occurred, e.g. 2^-10000. " );

	if( a->type == TLOSS )
		strcpy( error_message,
				  "A Loss of Significant Digits has occurred, e.g. sin(10*10^70)." );


	hide_mouse();

	x1 = 50*xratio/640;
	x2 = 80*xratio/640+textwidth( error_message );
	y1 = 150*yratio/480;
	y2 = 170*yratio/480+(3*textheight("T"));

	if( (graphics_buffer=malloc(imagesize(x1,y1,x2,y2)))==NULL )
		allocation_error( "matherr" );

	getimage( x1, y1, x2, y2, graphics_buffer );
	box( 2, x1, y1, x2, y2 );

	setcolor( RED );
	outtextxy( x1+(13*xratio/640), y1+(12*yratio/480), "ERROR :" );

	setcolor( BLUE );
	outtextxy( x1+(13*xratio/640), y1+(12*yratio/480)+(2*textheight("T")),
			   error_message );

	getch();

	putimage( x1, y1, graphics_buffer, COPY_PUT );
	free( graphics_buffer );

	setfillstyle( SOLID_FILL, LIGHTCYAN );
	bar( 4*xratio/640, 452*yratio/480,
		  textwidth( "-0.000000000" )+6*xratio/640, 476*yratio/480 );


	show_mouse();

	menu_system();
}

int point_not_defined_error()
{
	char *graphics_buffer, answer;
	int x1, y1, x2, y2;

	hide_mouse();

	x1 = 50*xratio/640;
	x2 = 80*xratio/640+textwidth( "ERROR : This value is undefined on the graph");
	y1 = 150*yratio/480;
	y2 = 170*yratio/480+(3*textheight("T"));

	if( (graphics_buffer=malloc(imagesize(x1,y1,x2,y2))) == NULL )
		allocation_error( "error_message" );

	getimage( x1, y1, x2, y2, graphics_buffer );
	box( 2, x1, y1, x2, y2 );

	setcolor( RED );
	outtextxy( x1+(13*xratio/640), y1+(12*yratio/480),
				  "ERROR : " );

	setcolor( BLUE );
	outtextxy( x1+(13*xratio/640)+textwidth( "ERROR : " ),
				  y1+(12*yratio/480), "This value is undefined on the graph" );

	setcolor( RED );
	outtextxy( x1+(13*xratio/640), y1+(12*yratio/480)+(2*textheight("T")),
			 "C" );

	setcolor( BLUE );
	outtextxy( x1+(13*xratio/640)+textwidth("C"),
				  y1+(12*yratio/480)+(2*textheight("T")), "ontinue or " );

	setcolor( RED );
	outtextxy( x1+(13*xratio/640)+textwidth("Continue or " ),
				  y1+(12*yratio/480)+(2*textheight("T")), "A" );

	setcolor( BLUE );
	outtextxy( x1+(13*xratio/640)+textwidth("Continue or A" ),
				  y1+(12*yratio/480)+(2*textheight("T")), "bort ?" );

	do
	{
		answer = toupper(getch());
	}
	while( answer != 'C' && answer != 'A' );


	putimage( x1, y1, graphics_buffer, COPY_PUT );
	free( graphics_buffer );

	show_mouse();

	setcolor( MAGENTA );


	if( toupper(answer) == 'C' )
		return YES;
	else
		return NO;
}

zoom_in()
{
	int mouse_test, corner_flag;
	double left, right, top, bottom;
	double mouse_x, mouse_y, new_mouse_x, new_mouse_y;
	double new_minimum_x, new_minimum_y,  new_maximum_x, new_maximum_y;
	double x1, x2, y1, y2;
	char *graphics_buffer_one, *graphics_buffer_two;
	char *graphics_buffer_three, *graphics_buffer_four;
	union REGS inregs, outregs;


	data_saved_flag = NO;


	if( original_formula[0] == '\0' )
	{
		display_error_message( 50*xratio/640, 130*yratio/480,
			"A graph has not been defined. Select 'New Graph' option", "continue" );
		return;
	}


	left   = textwidth( "-0.000000000" )+(10*xratio/640);
	right  = xratio-1;
	top    = (19*yratio/480)+textheight("T");
	bottom = (450*yratio/480)-(2*textheight("T"));


	initialise_mouse();
	show_mouse();

	do
	{
		inregs.x.ax = 0x03;
		int86( 0x33, &inregs, &outregs );
		mouse_test = outregs.x.bx;
	}
	while( mouse_test == NO );


	mouse_x = outregs.x.cx;
	mouse_y = outregs.x.dx;
	new_mouse_x = mouse_x;
	new_mouse_y = mouse_y;


	if( mouse_x < left || mouse_x > right || mouse_y < top || mouse_y > bottom )
		return;


	setcolor( RED );


	do
	{
		inregs.x.ax = 0x03;
		int86( 0x33, &inregs, &outregs );
		mouse_test = outregs.x.bx;
		new_mouse_x = outregs.x.cx;
		new_mouse_y = outregs.x.dx;

		if( new_mouse_x < left )
			new_mouse_x = left;

		if( new_mouse_x > right )
			new_mouse_x = right;

		if( new_mouse_y < top )
			new_mouse_y = top;

		if( new_mouse_y > bottom )
			new_mouse_y = bottom;


		if( (graphics_buffer_one = malloc(imagesize(0,0,639,5))) == NULL )
			allocation_error( "zoom_in" );

		if( (graphics_buffer_two = malloc(imagesize(0,0,639,5))) == NULL )
			allocation_error( "zoom_in" );

		if( (graphics_buffer_three = malloc(imagesize(0,0,639,5))) == NULL )
			allocation_error( "zoom_in" );

		if( (graphics_buffer_four = malloc(imagesize(0,0,639,5))) == NULL )
			allocation_error( "zoom_in" );


		hide_mouse();


		if( (new_mouse_x > mouse_x) && (new_mouse_y > mouse_y) )
		{
			getimage( mouse_x, mouse_y, new_mouse_x, mouse_y,
						 graphics_buffer_one );
			getimage( mouse_x, mouse_y, mouse_x, new_mouse_y,
						 graphics_buffer_two );
			getimage( mouse_x, new_mouse_y, new_mouse_x, new_mouse_y,
						 graphics_buffer_three );
			getimage( new_mouse_x, mouse_y, new_mouse_x, new_mouse_y,
						 graphics_buffer_four );

			rectangle( mouse_x, mouse_y, new_mouse_x, new_mouse_y );

			putimage( mouse_x, mouse_y, graphics_buffer_one, COPY_PUT );
			putimage( mouse_x, mouse_y, graphics_buffer_two, COPY_PUT );
			putimage( mouse_x, new_mouse_y, graphics_buffer_three, COPY_PUT );
			putimage( new_mouse_x, mouse_y, graphics_buffer_four, COPY_PUT );

			corner_flag = 1;
		}

		if( (new_mouse_x < mouse_x) && (new_mouse_y < mouse_y) )
		{
			getimage( new_mouse_x, new_mouse_y, mouse_x, new_mouse_y,
						 graphics_buffer_one );
			getimage( new_mouse_x, new_mouse_y, new_mouse_x, mouse_y,
						 graphics_buffer_two );
			getimage( new_mouse_x, mouse_y, mouse_x, mouse_y,
						 graphics_buffer_three );
			getimage( mouse_x, new_mouse_y, mouse_x, mouse_y,
						 graphics_buffer_four );

			rectangle( mouse_x, mouse_y, new_mouse_x, new_mouse_y );

			putimage( new_mouse_x, new_mouse_y, graphics_buffer_one, COPY_PUT );
			putimage( new_mouse_x, new_mouse_y, graphics_buffer_two, COPY_PUT );
			putimage( new_mouse_x, mouse_y, graphics_buffer_three, COPY_PUT );
			putimage( mouse_x, new_mouse_y, graphics_buffer_four, COPY_PUT );

			corner_flag = 2;
		}

		if( (new_mouse_x < mouse_x) && (new_mouse_y > mouse_y) )
		{
			getimage( new_mouse_x, mouse_y, mouse_x, mouse_y,
						 graphics_buffer_one );
			getimage( new_mouse_x, mouse_y, new_mouse_x, new_mouse_y,
						 graphics_buffer_two );
			getimage( new_mouse_x, new_mouse_y, mouse_x, new_mouse_y,
						 graphics_buffer_three );
			getimage( mouse_x, mouse_y, mouse_x, new_mouse_y,
						 graphics_buffer_four );

			rectangle( mouse_x, mouse_y, new_mouse_x, new_mouse_y );

			putimage( new_mouse_x, mouse_y, graphics_buffer_one, COPY_PUT );
			putimage( new_mouse_x, mouse_y, graphics_buffer_two, COPY_PUT );
			putimage( new_mouse_x, new_mouse_y, graphics_buffer_three, COPY_PUT );
			putimage( mouse_x, mouse_y, graphics_buffer_four, COPY_PUT );

			corner_flag = 3;
		}

		if( (new_mouse_x > mouse_x) && (new_mouse_y < mouse_y) )
		{
			getimage( mouse_x, new_mouse_y, new_mouse_x, new_mouse_y,
						 graphics_buffer_one );
			getimage( mouse_x, new_mouse_y, mouse_x, mouse_y,
						 graphics_buffer_two );
			getimage( mouse_x, mouse_y, new_mouse_x, mouse_y ,
						 graphics_buffer_three );
			getimage( new_mouse_x, new_mouse_y, new_mouse_x, mouse_y,
						 graphics_buffer_four );

			rectangle( mouse_x, mouse_y, new_mouse_x, new_mouse_y );

			putimage( mouse_x, new_mouse_y, graphics_buffer_one, COPY_PUT );
			putimage( mouse_x, new_mouse_y, graphics_buffer_two, COPY_PUT );
			putimage( mouse_x, mouse_y, graphics_buffer_three, COPY_PUT );
			putimage( new_mouse_x, new_mouse_y, graphics_buffer_four, COPY_PUT );

			corner_flag = 4;
		}


		free( graphics_buffer_one ); free( graphics_buffer_two );
		free( graphics_buffer_three ); free( graphics_buffer_four );


		show_mouse();
	}
	while( mouse_test != NO );


	if( corner_flag == 1 )
	{
		x1 = mouse_x; x2 = new_mouse_x; y1 = new_mouse_y; y2 = mouse_y;
	}

	if( corner_flag == 2 )
	{
		x1 = new_mouse_x; x2 = mouse_x; y1 = mouse_y; y2 = new_mouse_y;
	}

	if( corner_flag == 3 )
	{
		x1 = new_mouse_x; x2 = mouse_x; y1 = new_mouse_y; y2 = mouse_y;
	}

	if( corner_flag == 4 )
	{
		x1 = mouse_x; x2 = new_mouse_x; y1 = mouse_y; y2 = new_mouse_y;
	}


	if( x1 == x2 || y1 == y2 )
		return;


	new_minimum_x = (x1-left)/(right-left)*(5*division_x)+minimum_x;
	new_minimum_y = (minimum_y+5*division_y)-((y1-top)/(bottom-top)*(5*division_y));
	new_maximum_x = (x2-left)/(right-left)*(5*division_x)+minimum_x;
	new_maximum_y = (minimum_y+5*division_y)-((y2-top)/(bottom-top)*(5*division_y));


	if( coordinate_stack_pointer == 18 )
		display_warning_message( 150*xratio/640, 130*yratio/480,
			"Further zooms will not be recorded    ",
			"Press 'y' to continue" );


	if( check_for_accuracy( new_minimum_x, new_maximum_x ) == NO ||
		check_for_accuracy( new_minimum_y, new_maximum_y ) == NO )
	{
		display_error_message( 150*xratio/640, 130*yratio/480,
			"The graph cannot be drawn to this accuracy", "continue" );
		return;
	}


	if( coordinate_stack_pointer <= 19 )
	{
		coordinate_stack[coordinate_stack_pointer][0] = minimum_x;
		coordinate_stack[coordinate_stack_pointer][1] = minimum_x+5*division_x;
		coordinate_stack[coordinate_stack_pointer][2] = minimum_y;
		coordinate_stack[coordinate_stack_pointer][3] = minimum_y+5*division_y;
		coordinate_stack_pointer++;
	}


	draw_graph( new_minimum_x, new_maximum_x, new_minimum_y,
					new_maximum_y );

}

zoom_out()
{
	data_saved_flag = NO;


	if( coordinate_stack_pointer == 0 )
	{
		display_error_message( 150*xratio/640, 130*yratio/480,
			"Unable to zoom out            ", "continue" );
		return;
	}


	coordinate_stack_pointer--;


	draw_graph( coordinate_stack[coordinate_stack_pointer][0],
					coordinate_stack[coordinate_stack_pointer][1],
					coordinate_stack[coordinate_stack_pointer][2],
					coordinate_stack[coordinate_stack_pointer][3]  );
}

save_graph( int ask_for_file_name )
{
	char *graphics_buffer_one, *graphics_buffer_two;
	char text_store[50];
	int x1, y1, x2, y2, write_flag=NO;
	FILE *file_pointer;
	unsigned int hash=0;

	if( original_formula[0] == '\0' )
	{
		display_error_message( 50*xratio/640, 130*yratio/480,
			"A graph has not been defined. Select 'New graph' option", "continue" );

		return;
	}


	if( file_name[0] == '\0' )
		ask_for_file_name = YES;


	graphics_buffer_two = summarise_data();


	x1 = 50*xratio/640;
	x2 = 590*xratio/640;
	y1 = 120*yratio/480+8*textheight("T");
	y2 = 140*yratio/480+9*textheight("T");


	if( (graphics_buffer_one = malloc(imagesize(x1,y1,x2,y2))) == NULL )
		allocation_error( "save_graph" );


	getimage( x1, y1, x2, y2, graphics_buffer_one );
	box( 2, x1, y1, x2, y2 );


	setcolor( BLUE );
	if( ask_for_file_name == NO )
	{
		sprintf( text_store, "Save to '%s' (Y/N)", file_name );

		outtextxy( x1+(10*xratio/640), y1+(11*yratio/480),
					  text_store );

		if( toupper( getch() ) == 'Y' );
			write_flag = YES;
	}
	else
	{
		text_input( "Filename : ", x1+(10*xratio/640),
						y1+(11*yratio/480), 45, file_name );

		if( file_name[0] != '\0' )
			write_flag = YES;
	}

	putimage( x1, y1, graphics_buffer_one, COPY_PUT );
	putimage( 50*xratio/640, 100*yratio/480, graphics_buffer_two, COPY_PUT );


	free( graphics_buffer_two );


	if( write_flag == YES )
	{
		if( ask_for_file_name == YES )
			if( (file_pointer = fopen( file_name, "rb" )) != NULL )
			{
				fclose( file_pointer );
				if( (display_warning_message( 130*xratio/640, 130*yratio/480,
					 "The file you have specified already exists.",
					 "Do you wish to overwrite it (Y/N) ?" ) == NO ) )
				{
					free( graphics_buffer_one );
					return;
				}
			}


		hash = hash_total();


		if( (file_pointer = fopen( file_name, "wb")) == NULL )
			display_error_message( 150*xratio/640, y1,
				"Unable to save data using requested path", "continue" );
		else
		{
			fwrite( original_formula, sizeof(original_formula), 1, file_pointer );
			fwrite( &minimum_x, sizeof(minimum_x), 1, file_pointer );
			fwrite( &division_x, sizeof(division_x), 1, file_pointer );
			fwrite( &minimum_y, sizeof(minimum_y), 1, file_pointer );
			fwrite( &division_y, sizeof(division_y), 1, file_pointer );
			fwrite( &speed, sizeof(speed), 1, file_pointer );
			fwrite( &radians_flag, sizeof(radians_flag), 1, file_pointer );
			fwrite( &grid_flag, sizeof(grid_flag), 1, file_pointer );
			fwrite( &hash, sizeof(hash), 1, file_pointer );

			fclose( file_pointer );

			getimage( 150*xratio/640, 150*yratio/480, 590*xratio/640,
					170*yratio/480+2*textheight("T"), graphics_buffer_one );

			box( 2, 150*xratio/640, 150*yratio/480,
				  170*xratio/640+textwidth( "The graph has been saved successfully"),
				  170*yratio/480+2*textheight("T") );

			outtextxy( 162*xratio/640, 160*yratio/480,
						  "The graph has been saved successfully" );
			outtextxy( 162*xratio/640, 162*yratio/480+textheight("T"),
						  "Press any key to continue" );

			getch();

			putimage( 150*xratio/640, 150*yratio/480, graphics_buffer_one, COPY_PUT );

			data_saved_flag = YES;
		}
	}

	free( graphics_buffer_one );
}

char * summarise_data()
{
	char *graphics_buffer, text_store[100];
	int x1, y1, x2, y2;


	hide_mouse();


	x1 = 50*xratio/640;
	x2 = 590*xratio/640;
	y1 = 100*yratio/480;
	y2 = 120*yratio/480+6*textheight("T");


	if( (graphics_buffer=malloc(imagesize(x1,y1,x2,y2))) == NULL )
		allocation_error( "summarise_data" );


	getimage( x1, y1, x2, y2, graphics_buffer );
	box( 2, x1, y1, x2, y2 );

	setcolor( BLUE );

	sprintf( text_store, "Formula : %s", original_formula );
	outtextxy( x1+(10*xratio/640), y1+(10*yratio/480), text_store );

	sprintf( text_store, "Minimum x value : %f", minimum_x );
	outtextxy( x1+(10*xratio/640),
			   y1+(10*yratio/480)+2*(textheight("T")), text_store );

	sprintf( text_store, "Maximum x value : %f", minimum_x+5*division_x );
	outtextxy( x1+(10*xratio/640),
			   y1+(10*yratio/480)+3*(textheight("T")), text_store );

	sprintf( text_store, "Minimum y value : %f", minimum_y );
	outtextxy( x1+(10*xratio/640),
			   y1+(10*yratio/480)+4*textheight("T"), text_store );

	sprintf( text_store, "Maximum y value : %f", minimum_y+5*division_y );
	outtextxy( x1+(10*xratio/640),
			   y1+(10*yratio/480)+5*textheight("T"), text_store );


	return graphics_buffer;
}


load_graph()
{
	int x1, y1, x2, y2;
	unsigned int hash;
	FILE *file_pointer;
	char *graphics_buffer, dummy_error_message[50];
	unsigned char formula_store[150];


	hide_mouse();


	if( original_formula[0] != '\0' )
		if( (display_warning_message( x1+10*xratio/640, y1,
			"The current data will be lost if a graph is loaded",
			"Do you still wish to continue (Y/N) ?" )) == NO )

			return;


	x1 = 50*xratio/640;
	x2 = 590*xratio/640;
	y1 = 150*yratio/480;
	y2 = 170*yratio/480+2*textheight("T");


	if( (graphics_buffer = malloc(imagesize(x1,y1,x2,y2))) == NULL )
		allocation_error( "load_graph" );


	getimage( x1, y1, x2, y2, graphics_buffer );

	box( 2, x1, y1, x2, y2-textheight("T") );

	setcolor( BLUE );
	text_input( "Filename : ", x1+(10*xratio/640),
					y1+(11*yratio/480), 45, file_name );


	putimage( x1, y1, graphics_buffer, COPY_PUT );


	if( file_name[0] != '\0' )
	{
		if( (file_pointer = fopen( file_name, "rb")) == NULL )
			display_error_message( 80*xratio/640, y1,
				"The file cannot be found using the requested path", "continue" );
		else
		{
			fread( original_formula, sizeof(original_formula), 1, file_pointer );
			fread( &minimum_x, sizeof(minimum_x), 1, file_pointer );
			fread( &division_x, sizeof(division_x), 1, file_pointer );
			fread( &minimum_y, sizeof(minimum_y), 1, file_pointer );
			fread( &division_y, sizeof(division_y), 1, file_pointer );
			fread( &speed, sizeof(speed), 1, file_pointer );
			fread( &radians_flag, sizeof(radians_flag), 1, file_pointer );
			fread( &grid_flag, sizeof(grid_flag), 1, file_pointer );
			fread( &hash, sizeof(hash), 1, file_pointer );

			data_saved_flag = YES;


			if( hash_total() != hash )
			{
				display_error_message( 80*xratio/640, y1,
					"The data in the requested file cannot be loaded", "continue" );

				strcpy( original_formula, "" );
				speed = 5;
				radians_flag = NO;
				grid_flag = YES;
				data_saved_flag = YES;
				setfillstyle( SOLID_FILL, LIGHTBLUE );
				bar( 0,0, xratio-1, yratio-1 );
				menu_headings();
			}
			else
			{
				strcpy( formula_store, original_formula );

				translate_formula( formula_store, dummy_error_message );

				draw_graph( minimum_x, minimum_x+5*division_x, minimum_y,
								minimum_y+5*division_y );
			}
		}
	}

	fclose( file_pointer );

	show_mouse();

	free( graphics_buffer );
}

allocation_error( char *function )
{
	closegraph();

	printf( "\n\nThe function %s has run out of memory\n\n", function );
	printf( "Please use a system with more free memory.\n\n" );

	exit(1);
}

box( int border_lines, int x1, int y1, int x2, int y2 )
{
	int current_colour = getcolor();


	setfillstyle( SOLID_FILL, WHITE );

	bar( x1, y1, x2, y2 );

	setcolor( DARKGRAY );

	rectangle( x1, y1, x2, y2 );


	if( border_lines == 2 )
		rectangle( x1+(2*xratio/640), y1+(3*yratio/480),
			   x2-(2*xratio/640), y2-(3*yratio/480) );


	setcolor( current_colour );
}

print_dots( int number_of_pins, int dot_one, int dot_two, int dot_three )
{
	int loop;


	biosprint( 0, 27, 0 );
	biosprint( 0, 42, 0 );


	if( number_of_pins == 9 )
	{
		biosprint( 0, 6, 0 );
		biosprint( 0, 1, 0 );
	}
	else
	{
		biosprint( 0,40,0 );
		biosprint( 0,4, 0 );
	}


	biosprint( 0, 0, 0 );


	if( number_of_pins == 9 )
		biosprint( 0, dot_one, 0 );
	else
		for( loop=1; loop <= 4; loop++ )
		{
			biosprint( 0, dot_one, 0 );
			biosprint( 0, dot_two, 0 );
			biosprint( 0, dot_three, 0 );
		}
}

print_graph( int number_of_pins )
{
	int x,y=0, dot_one=0, dot_two=0, dot_three=0, loop, pixel, start_y=12;
	int y_increment, loop_increment, dot_flag;


	if( original_formula[0] == '\0' )
	{
		display_error_message( 50*xratio/640, 150*yratio/480,
		 "A graph has not been defined. Select 'New graph' option", "continue" );
		return;
	}


	biosprint( 1,0,0 );

	delay( 2000 );


	if( biosprint( 2,0,0 ) != 144 )
	{
		display_error_message( 150*xratio/640, 150*yratio/480,
		  "There is a fault with the printer  ", "continue" );
		return;
	}


	if( number_of_pins == 24 )
	{
		y_increment = 12; loop_increment = 4; dot_flag = 2;
	}
	else
	{
		y_increment = 8;  loop_increment = 8; dot_flag = 1;
	}


	hide_mouse();


	setfillstyle( SOLID_FILL, LIGHTBLUE );
	bar( 0, 0, xratio-1, 10*yratio/480+textheight("T") );
	setcolor( WHITE );
	outtextxy( 12*xratio/640, 10*yratio/480,
				  "Printing in progress. Press 'Q' to terminate..." );


	biosprint( 0,27,0 );
	biosprint( 0,51,0 );
	biosprint( 0,25,0 );


	if( yratio == 350 )
		start_y = 14;


	for( y=start_y ; y < yratio; y+=y_increment )
	{
		for( x=0; x < xratio; x++ )
		{
			for( loop=0; loop != loop_increment; loop++ )
			{
				if( kbhit() )
					if( toupper(getch()) == 'Q' )
					{
						setfillstyle( SOLID_FILL, LIGHTBLUE );
						bar( 0,0, xratio-1, 10*yratio/480+textheight("T") );
						menu_headings();
						return;
					}

				pixel = getpixel( x, loop+y );

				if( pixel==BLACK || pixel==MAGENTA || pixel==BLUE ||
					pixel==DARKGRAY )
				{
					dot_one += pow( 2, 7-loop*dot_flag );

					if( number_of_pins == 24 )
						dot_one += pow( 2, 6-loop*2 );
					else
						continue;
				}

				pixel = getpixel( x, loop+y+4 );

				if( pixel==BLACK || pixel==MAGENTA || pixel==BLUE ||
					pixel==DARKGRAY )
				{
					dot_two += pow( 2, 7-loop*2 );
					dot_two += pow( 2, 6-loop*2 );
				}

				pixel = getpixel( x, loop+y+8 );

				if( pixel==BLACK || pixel==MAGENTA || pixel==BLUE ||
					pixel==DARKGRAY )
				{
					dot_three += pow( 2, 7-loop*2 );
					dot_three += pow( 2, 6-loop*2 );
				}
			}

			print_dots( number_of_pins, dot_one, dot_two, dot_three );

			dot_one = 0; dot_two = 0; dot_three = 0;
		}

		biosprint( 0, 10, 0 );
	}

	setfillstyle( SOLID_FILL, LIGHTBLUE );
	bar( 0,0, xratio-1, 10*yratio/480+textheight("T") );
	menu_headings();

	show_mouse();
}

tidy_the_formula()
{
	int loop;
	unsigned char formula_buffer[150];


	for( loop=0; loop < strlen( original_formula ); loop++ )
		if( isupper( original_formula[loop] ) )
			original_formula[loop] = tolower( original_formula[loop] );


	for( loop=0; loop < strlen( original_formula ); loop++ )
		if( original_formula[loop] == ' ' )
		{
			strcpy( formula_buffer, sub_string( original_formula, 1, loop ) );
			strcat( formula_buffer, sub_string( original_formula, loop+2,
								  strlen(original_formula)-loop-1 ) );
			strcpy( original_formula, formula_buffer );
			loop = -1;
		}



	for( loop=0; loop < strlen( original_formula ); loop++ )
	{
		if( original_formula[loop] == '.' )
		{
			if( (1-(isdigit( original_formula[loop-1]) && loop != 0))
			    && original_formula[loop-1] != '.' )
			{
				if( loop == 0 )
				{
					strcpy( formula_buffer, "0" );
					strcat( formula_buffer, original_formula );
					strcpy( original_formula, formula_buffer );
				}
				else
				{
					strcpy( formula_buffer, sub_string(original_formula, 1, loop ) );
					strcat( formula_buffer, "0" );
					strcat( formula_buffer, sub_string(original_formula, loop+1,
							strlen(original_formula)-loop ) );
					strcpy( original_formula, formula_buffer );
				}

				loop++;
			}


			if( (1-(isdigit( original_formula[loop+1] ) &&
				 loop != strlen(original_formula)-1 ))
			    && original_formula[loop+1] != '.' )
			{
				if( loop == strlen(original_formula)-1 )
					strcat( original_formula, "0" );
				else
				{
					strcpy( formula_buffer, sub_string(original_formula,1,loop+1 ) );
					strcat( formula_buffer, "0" );
					strcat( formula_buffer, sub_string(original_formula,loop+2,
								 strlen(original_formula)-loop-1) );
					strcpy( original_formula, formula_buffer );
				}
			}
		}
	}


	if( original_formula[1] == '=' )
		strcpy( original_formula, sub_string( original_formula, 3,
				strlen(original_formula)-2 ) );

}


int check_for_digits( char *string )
{
	int loop, valid_number_flag=NO, start_point=0;
	int decimal_points=0;


	if( string[0] == '-' )
		start_point++;


	for( loop=start_point; loop < strlen( string ); loop++ )
		if( string[loop] == '.' )
			decimal_points++;


	if( decimal_points > 1 )
		return NO;


	for( loop=start_point; loop < strlen( string ); loop++ )
		if( isdigit( string[loop] ) || (string[loop] == '.' && string[loop+1]!='.'))
			valid_number_flag = YES;
		else
		{
			valid_number_flag = NO;
			break;
		}


	 return valid_number_flag;
}

display_error_message( int x1, int y1, char *message, char *endword )
{
	char *graphics_buffer, text_store[50];
	int x2, y2;


	x2 = x1+textwidth( "ERROR : " )+textwidth( message )+25*xratio/640;
	y2 = y1+(20*yratio/480)+(2*textheight("T"));


	hide_mouse();


	if( (graphics_buffer = malloc(imagesize(x1,y1,x2,y2))) == NULL )
		allocation_error( "display_error_message" );


	getimage( x1, y1, x2, y2, graphics_buffer );

	box( 2, x1, y1, x2, y2 );

	setcolor( RED );
	outtextxy( x1+10*xratio/640, y1+12*yratio/480, "ERROR : " );


	setcolor( BLUE );
	outtextxy( x1+10*xratio/640+textwidth("ERROR : "), y1+12*yratio/480,
				  message );

	strcpy( text_store, "Press any key to " );
	strcat( text_store, endword );
	outtextxy( x1+10*xratio/640+textwidth("ERROR : "),
				  y1+12*yratio/480+textheight("T"), text_store );


	getch();


	putimage( x1, y1, graphics_buffer, COPY_PUT );
	free( graphics_buffer );


	show_mouse();
}

int test_the_formula( char *error_message )
{
	int loop, left_brackets=0, right_brackets=0;
	char x;


	if( strlen(original_formula) == 0 )
	{
		strcpy( error_message, "Please type in the formula to be plotted" );
		return NO;
	}


	for( loop=0; loop != strlen(original_formula); loop++ )
	{
		x = original_formula[loop];

		if( (x<40) || (x>122) || ((x>57)&&(x<94)) || (x==96) ||
			(x==44) || (x==95) )
		{
			sprintf( error_message,
				"Invalid character '%c' found in formula.", x );
			return NO;
		}
	}


	for( loop=0; loop != strlen(original_formula)-1; loop++ )
	{
		if( original_formula[loop] == '.' && original_formula[loop+1]=='.' )
		{
			strcpy( error_message, "There are two adjacent decimal points." );
			return NO;
		}
	}


	for( loop=0; loop != strlen(original_formula); loop++ )
	{
		if( original_formula[loop] == '(' )
			left_brackets++;
		if( original_formula[loop] == ')' )
			right_brackets++;
	}


	if( left_brackets != right_brackets )
	{
		strcpy( error_message, "The parentheses do not balance." );
		return NO;
	}


	return YES;
}

add_multiply_operator( unsigned char *formula )
{
	int loop;
	unsigned char formula_store[150];


	for( loop=0; loop != strlen(formula); loop++ )
	{

		if( (isupper(formula[loop])) || (formula[loop]=='x') ||
			(formula[loop]==')') )
		{

			if( (formula[loop+1]=='(') ||
				(formula[loop+1]>=128 && formula[loop+1]<=254) ||
				(formula[loop+1]=='x')||
				(formula[loop]=='x' && isupper(formula[loop+1])) )
			{

				strcpy( formula_store, sub_string(formula, 1, loop+1 ) );
				strcat( formula_store, "*" );
				strcat( formula_store, sub_string(formula, loop+2,
						  strlen(formula)-loop-1) );
				strcpy( formula, formula_store );

			}
		}
	}
}

unsigned int hash_total()
{
	unsigned int total=0;
	int loop;
	char *xmin_ptr, *ymin_ptr, *xdiv_ptr, *ydiv_ptr;


	for( loop=0; loop < strlen(original_formula); loop++ )
		total += original_formula[loop];


	xmin_ptr = &minimum_x;
	ymin_ptr = &minimum_y;
	xdiv_ptr = &division_x;
	ydiv_ptr = &division_y;


	for( loop=0; loop <= 7; loop++ )
	{
		total += *(char *)xmin_ptr;
		total += *(char *)ymin_ptr;
		total += *(char *)xdiv_ptr;
		total += *(char *)ydiv_ptr;

		xmin_ptr++; ymin_ptr++; xdiv_ptr++; ydiv_ptr++;
	}


	total += speed;
	total += radians_flag;
	total += grid_flag;


	return total;
}

int check_for_accuracy( double minimum, double maximum )
{
	int sig_figures=1;
	char exponent_buffer[30];
	int min_exponent, div_exponent;



	sprintf( exponent_buffer, "%.9e", (maximum-minimum)/5 );
	div_exponent = atoi( exponent_buffer+11 );


	sprintf( exponent_buffer, "%.9e", fabs( minimum ) );
	min_exponent = atoi( exponent_buffer+11 );



	if( min_exponent > 0 )
		sig_figures += min_exponent;
	if( div_exponent < 0 )
		sig_figures += abs( div_exponent );



	if( ((maximum-minimum)/5 < 0.000000001 ) ||
		(sig_figures > 10) )

		return NO;
	else
		return YES;
}

int display_warning_message( int x1, int y1, char *line_one, char *line_two )
{
	char *graphics_buffer, answer;
	int x2, y2;


	x2 = x1+textwidth( "WARNING : " )+textwidth( line_one )+25*xratio/640;
	y2 = y1+(20*yratio/480)+(2*textheight("T"));


	hide_mouse();


	if( (graphics_buffer = malloc(imagesize(x1,y1,x2,y2))) == NULL )
		allocation_error( "display_warning_message" );


	getimage( x1, y1, x2, y2, graphics_buffer );

	box( 2, x1, y1, x2, y2 );

	setcolor( RED );
	outtextxy( x1+10*xratio/640, y1+12*yratio/480, "WARNING : " );

	setcolor( BLUE );

	outtextxy( x1+10*xratio/640+textwidth( "WARNING : " ), y1+12*yratio/480,
			 line_one );
	outtextxy( x1+10*xratio/640+textwidth( "WARNING : " ),
			 y1+12*yratio/480+textheight("T"), line_two );


	do
	{
		answer = toupper( getch() );
	}
	while( answer != 'Y' && answer != 'N'  );


	putimage( x1, y1, graphics_buffer, COPY_PUT );

	free( graphics_buffer );

	show_mouse();


	if( answer == 'Y' )
		return YES;
	else
		return NO;
}
