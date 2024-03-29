//====================================================================================
//     The MIT License (MIT)
//
//     Copyright (c) 2011 Kapparock LLC
//
//     Permission is hereby granted, free of charge, to any person obtaining a copy
//     of this software and associated documentation files (the "Software"), to deal
//     in the Software without restriction, including without limitation the rights
//     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//     copies of the Software, and to permit persons to whom the Software is
//     furnished to do so, subject to the following conditions:
//
//     The above copyright notice and this permission notice shall be included in
//     all copies or substantial portions of the Software.
//
//     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//     THE SOFTWARE.
//====================================================================================
#ifndef APS_DATA_SERVICE_IMPL_HPP_
#define APS_DATA_SERVICE_IMPL_HPP_

namespace aps {
namespace data_service{
		using namespace std;
		struct SyncInfo;

		using SyncInfoPtr 		= unique_ptr<SyncInfo>;
		using MsgRcvr_		= tuple<PredWrapper, CbWrapper, SyncInfoPtr>;
		using HandlerBuffer 	=	forward_list<MsgRcvr_>;
		HandlerBuffer buffer__;
		mutex buffer__Mutex;

		class TxHandle
		{
		private:
			struct Data;
			Data* data;
		public:
			TxHandle(PredWrapper&& pred,CbWrapper&& cb);
			void arm();
			bool wait_for(int64_t micro_sec);
			void remove();
			~TxHandle();
		};

		struct SyncInfo
		{
			mutex m{};
			condition_variable cv{};
			bool sync	{false};
			bool rdy	{false};
			SyncInfo(){}
			void arm()
			{
				sync = true;
				rdy = false;
			}
			bool wait_for(int64_t wait_time_us)
			{
				if (!sync)
					return false;
				unique_lock<mutex> lk{m};
				return cv.wait_for(lk, chrono::microseconds(wait_time_us), [this]{return rdy;});
			}
			void disarm()
			{
				if (!sync)
					return;
				sync 	= false;
				rdy 	= true;
				unique_lock<mutex> lk{m};
				cv.notify_one();
			}
		};

		struct TxHandle::Data
		{
			HandlerBuffer::iterator it_;
		};

		TxHandle::TxHandle(PredWrapper&& pred, CbWrapper&& cb):data{new Data{}}
		{
			buffer__Mutex.lock();
			data->it_ = buffer__.emplace_after(buffer__.before_begin(), move(pred),move(cb), SyncInfoPtr(new SyncInfo{}));
			buffer__Mutex.unlock();
		}
		TxHandle::~TxHandle()
		{
			delete data;
		}
		void TxHandle::arm()
		{
			get<2>(*data->it_)->arm();
		}
		bool TxHandle::wait_for(int64_t micro_sec)
		{
			return get<2>(*data->it_)->wait_for(micro_sec);
		}
		void TxHandle::remove()
		{
			buffer__Mutex.lock();
			swap(*(data->it_), buffer__.front());
			buffer__.pop_front();
			buffer__Mutex.unlock();
		}

		TxHandle registerHandlers(PredWrapper&& pred, CbWrapper&& cb)
		{
			return TxHandle{move(pred), move(cb)};
		}

		void	removeHandler(TxHandle& h)
		{
			h.remove();
		}

		void request(const AFMessage& msg)
		{
			sendAPSMessageStruct(const_cast<AFMessage*>(&msg));
		}

		void request(AFMessage& msg, PredWrapper&& pred, CbWrapper&& cb, int64_t wait_time_us)
		{
			//using namespace std::chrono;
			auto h = registerHandlers(move(pred),move(cb));
			h.arm();
			sendAPSMessageStruct(&msg);

			//auto t0 = steady_clock::now();
			if (h.wait_for(wait_time_us)){
				//LOG_MESSAGE("got it");
			}
//			else
//			{
//				LOG_MESSAGE("did not get it");
//			}
//			auto t1 = steady_clock::now();
//			LOG_MESSAGE("duration = %d ms", duration_cast<milliseconds>(t1 - t0).count());
			removeHandler(h);
		}

		bool indicate(AFMessage& msg)
		{
			bool served = false;
			buffer__Mutex.lock();
			for (auto& y : buffer__)
			{
				if (get<0>(y)(msg))
				{
					get<1>(y)(msg);
					//get<1>(y)(msg);
					get<2>(y)->disarm();
					served = true;
					break;
				}
			}
			buffer__Mutex.unlock();
			return served;
		}

		int apsMessageIndicate(AFMessage* message)
		{
			if (indicate(*message)) {	return 0; }
			if (Endpoint* x = selfptr()->endpointPtr(message->dstEndpoint()))
			{
				if (x->messageHandler != NULL)
					x->messageHandler(message);
				//else
				//{
					//x->handleAPSMessage(*message);
					//aps::data_service::indicate(*message);
				//}
			}

			return 0;
		}
		void init()
		{
			halSetAPSMessageHandler(apsMessageIndicate);
			//kapi::mtaf::register_AFMessage_cb(apsMessageIndicate);
			//mtaf::regi(apsMessageIndicate);
		}
	} //data_service
//	namespace data_service {
//		using namespace std;
//		using CbWrapperPacker   =  packaged_task<void(AFMessage&)>;
//		using MsgRcvr__ 				=  tuple<PredWrapper, CbWrapperPacker>;
//		forward_list<MsgRcvr__> buffer__;
//		mutex buffer__Mutex;
//
//		void dummyProcess(AFMessage& msg)
//		{
//			LOG_MESSAGE("hello bobo");
//		}
//
//		void request(const AFMessage& msg)
//		{
//			sendAPSMessageStruct(const_cast<AFMessage*>(&msg));
//		}


//		void request(AFMessage& msg, PredWrapper&& pred, CbWrapper&& cb)
//		{
//			buffer__Mutex.lock();
//			auto it = buffer__.emplace_after(buffer__.before_begin(), move(pred),CbWrapperPacker(cb));
//			buffer__Mutex.unlock();
//
//			auto fu = get<1>(*it).get_future();
//			sendAPSMessageStruct(&msg);
//			fu.wait_for(chrono::seconds(1));
//
//			buffer__Mutex.lock();
//			swap(*it, buffer__.front());
//			buffer__.pop_front();
//			buffer__Mutex.unlock();
//		}
//
//		bool indicate(AFMessage& msg)
//		{
//			bool served = false;
//			buffer__Mutex.lock();
//			for (auto& y : buffer__)
//			{
//				if (get<0>(y)(msg))
//				{
//					get<1>(y)(msg);
//					served = true;
//					break;
//				}
//			}
//			buffer__Mutex.unlock();
//			return served;
//		}
//
//		int apsMessageIndicate(AFMessage* message)
//		{
//			if (indicate(*message)) {	return 0; }
//			if (Endpoint* x = selfptr()->endpointPtr(message->dstEndpoint()))
//			{
//				if (x->messageHandler != NULL)
//					x->messageHandler(message);
//				//else
//				//{
//					//x->handleAPSMessage(*message);
//					//aps::data_service::indicate(*message);
//				//}
//			}
//
//			return 0;
//		}
//		void init()
//		{
//			halSetAPSMessageHandler(apsMessageIndicate);
//		}
//	} // __data_service
}
#endif
