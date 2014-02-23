#include<boost/mpl/list.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/begin_end.hpp>
#include<vector>
#include<utility>
#include <mutex>

#include "varcontainer.h"

using namespace artisoko::core::util;

namespace artisoko {
	namespace core {
		namespace engine {

			namespace {

				template<class part_t, int deltacnt = 1>
				struct part_and_delta {
					typename part_t part;
					mutable typename part_t::delta_t delta[deltacnt];
				};


				template<class part_t, int deltacnt = 1>
				struct processor {
					typedef part_t part_type;
					enum delta_count {
						count = deltacnt
					};

					// void process(const part_t& part, typename part_t::delta_t& delta) = 0;
				};

				/*enum pipeline_type {
					normal
					};*/

				struct default_pipeline_config {
					//enum type {
					//	value = pipeline_type::normal
					//};
				};

				template<class iterator_t, int delta_count>
				struct processor_op{
					iterator_t iter;
					processor_op(iterator_t iter) : iter(iter) {}
					template<class processor_t>
					void operator()(processor_t& processor) const {
						processor.inst.process(iter.part, iter.delta[delta_count]);
					}
				};

				
			}

			template<class processors_l, class pipe_config = default_pipeline_config, class container_t = std::vector<part_and_delta<boost::mpl::front<processors_l>::type::part_type, boost::mpl::front<processors_l>::type::count>> >
			struct pipeline {

				enum delta_count {
					count = boost::mpl::front<processors_l>::type::count
				};
				typedef processors_l processors_list;
				typedef typename boost::mpl::front<processors_l>::type::part_type part_t;
				typedef typename part_and_delta<part_t, count> part_and_delta_t;
				//typedef typename std::vector<part_and_delta_t>::iterator iterator_t;
				typedef typename container_t::iterator iterator_t;

			private:

				//template<class proc_l_iter> struct processor_instances {
				//	typedef typename boost::mpl::deref<proc_l_iter>::type processor_t;
				//	typedef processor_instances<typename boost::mpl::next<proc_l_iter>::type> next_t;
				//	processor_t processor;
				//	next_t next;
				//};
				//template<>struct processor_instances<typename boost::mpl::end<processors_l>::type >{};

				//processor_instances< typename boost::mpl::begin<processors_l>::type > processors;

				//template<class procs_iter>
				//void proc(part_and_delta_t& element, typename processor_instances<procs_iter>& processor) {
				//	//std::cout << typeid(boost::mpl::deref<procs_iter>::type).name() << std::endl;
				//	processor.processor.process(element.part, element.delta[0]);
				//	proc<boost::mpl::next<procs_iter>::type>(element, processor.next);
				//}

				//template<>
				//void proc<typename boost::mpl::end<processors_l>::type>(part_and_delta_t& element, typename processor_instances<typename boost::mpl::end<processors_l>::type>& processor) {
				//}

				varcontainer<processors_list> processors;

			public:

				void process(iterator_t start, iterator_t end, int delta_idx = 1) {
					for (iterator_t it = start; it != end; ++it) {
						processors.foreach(processor_op<part_and_delta_t, count>(*it));
						// proc<typename boost::mpl::begin<processors_l>::type>(*it, processors);
					}
				}

			};

		}
	}
}