void *malloc( int size, int size2 )
{
	return (void *)0;
}

int func_2 ( void )
{
	return 1;
}

int func_1 ( void )
{
	void *ptr = malloc( 20, 4 );
	return func_2();
}

int main ( void )
{
	void *ptr = malloc( 40, 3 );

	return func_1();
}
