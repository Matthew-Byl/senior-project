/**
 * Functions to create and validate Societies
 *  config structs.
 *
 * @author John Kloosterman
 * @date Feb. 7, 2013
 */

#include "config.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

SocietiesConfig config_generate_default_configuration( void )
{
	SocietiesConfig config;

	config.num_threads = 256;

	config.num_agents = 100;
	config.num_resources = 256;
	config.num_days = 50;
	config.num_minutes = 600;

	for ( int i = 0; i < 256; i++ )
	{
		config.resource_D[i] = 10;
		config.resource_n[i] = 10;
	}

	config.resource_epsilon = 1;
	config.max_experience = 200;
	config.min_effort = 3;
	config.max_effort = 9;

	config.menu_size = 5;
	config.num_trades = 5;

	return config;
}

bool config_verify_configuration( SocietiesConfig config )
{
	/// @XXX: ask OpenCL for the real number.
	if ( config.num_resources > 256 )
	{
		cerr << "There are more resource than the OpenCL workgroup size." << endl;
		return false;
	}

	// We assume there is one thread per resources.
	if ( config.num_threads != config.num_resources )
	{
		cerr << "There have to be the same number of threads as resources." << endl;
		return false;
	}

	// The menu_size^2 needs to be >= the number of threads.
	if ( (config.menu_size * config.menu_size ) > config.num_threads )
	{
		cerr << "The trading menu size squared needs to be less than the number of threads." << endl;
		return false;
	}

	return true;
}

// Create -D flags that can be passed to the OpenCL compiler.
string config_generate_compiler_flags( SocietiesConfig config )
{
	stringstream flags;

	// num_resources
	flags << "-D CONFIG_NUM_RESOURCES=" << config.num_resources << " ";

	// menu_size
	flags << "-D CONFIG_MENU_SIZE=" << config.menu_size << " ";

	// num_threads
	flags << "-D CONFIG_NUM_THREADS=" << config.num_threads << " ";

	// num_trades
	flags << "-D CONFIG_NUM_TRADES=" << config.num_trades << " ";

	return flags.str();
}
