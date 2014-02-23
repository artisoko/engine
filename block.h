#include<boost/mpl/list.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/begin_end.hpp>
#include<vector>
#include<utility>
#include <mutex>
#include<atomic>
#include "varcontainer.h"

using namespace artisoko::core::util;

namespace artisoko {
	namespace core {
		namespace engine {


			struct default_block_config {
				enum {
					block_size = 20
				};

				bool interrupted() {
					return false;
				}
			};

			struct default_engine_config {
				enum : unsigned long {
					max_iteration = -2
				};
			};

			struct pipe_work_op {
				int block_size;
				pipe_work_op(int block_size) : block_size(block_size) {}
				template<class pipe_t>
				bool operator()(pipe_t& pipe) const {
					// std::cout << typeid(pipe.inst).name() << std::endl << std::endl;
					// std::cout << typeid(pipe.additional.current_element).name() << std::endl << std::endl;

					// try to reserve next block
					// std::pair< > pipe.inst.reserve_next_block(block_size);

					return true;
				}
			};

			template<class pipe_t>
			struct additional_pipe_data {
				typename pipe_t::iterator_t current_element;
			};


			template<class pipes_l, class block_config_t = default_block_config, class engine_config = default_engine_config>
			struct block {

				typedef pipes_l pipes_list;

				typedef typename boost::mpl::front<pipes_l>::type::part_t part_t;
				typedef typename boost::mpl::front<pipes_l>::type::part_and_delta_t part_and_delta_t;
				typedef typename boost::mpl::front<pipes_l>::type::iterator_t iterator_t;

			private:

				const block_config_t& block_config;
				varcontainer<pipes_l, additional_pipe_data> pipes;

				std::atomic<unsigned long> iteration;
				std::mutex start_mtx;


				bool process_next_block() {
					return pipes.foreach_until_returns_true(pipe_work_op(block_config_t::block_size));
				}

				void init() {

				}

				iterator_t begin, end;
			public:

				block(const block_config_t& block_config = default_block_config()) : iteration(0), block_config(block_config) {}

				void start(long current_iteration, iterator_t begin, iterator_t end) {
					{
						std::unique_lock<std::mutex> lck(start_mtx);
						unsigned long old = iteration;
						if (old == current_iteration - 1 || old == engine_config::max_iteration && current_iteration == 0) {
							iteration = current_iteration;

						}
					}
					if (iteration == current_iteration) {
						while (process_next_block() && !block_config.interrupted());
					}

				}





			};
		}
	}
}