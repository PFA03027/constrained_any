/**
 * @file test_performance_constrained_any.cpp
 * @author Teruaki Ata (PFA03027@nifty.com)
 * @brief
 * @version 0.1
 * @date 2025-05-24
 *
 * @copyright Copyright (c) 2025, Teruaki Ata (PFA03027@nifty.com)
 *
 */

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <thread>

#include "constrained_any.hpp"

size_t test_performance_constrained_any( std::atomic<bool>* p_stop )
{
	yan::copyable_any sut1, sut2, sut3;
	size_t            count = 0;
	double            a     = 0.0;

	while ( p_stop->load() == false ) {
		++count;
		a += 0.1;

		sut1 = count;
		sut2 = a;
		sut3 = "test";
		sut3 = sut2;
		sut2 = sut1;
		sut1 = "test";
	}

	return count;
}

int main( void )
{
	std::packaged_task<size_t( std::atomic<bool>* )> perf_func( test_performance_constrained_any );
	std::future<size_t>                              ret = perf_func.get_future();
	std::atomic<bool>                                stop_flag( false );

	std::thread t( std::move( perf_func ), &stop_flag );

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	stop_flag = true;

	t.join();

	printf( "Performance count: %zu\n", ret.get() );

	return EXIT_SUCCESS;
}
