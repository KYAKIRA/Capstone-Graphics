#ifndef PROFILER_H
#define PROFILER_H

#include <vector>

namespace PizzaBox{
	class Profiler{
	public:
		explicit Profiler(const std::string name_);
		~Profiler();

		bool IsProfiling();

		void StartProfiling();
		double EndProfiling();

		double GetAverage();
		double GetNewestValue();
		long long GetIterations();

	private:
		const std::string name;
		long long startTime;
		std::vector<double> times; //In milliseconds
		bool isProfiling;
	};
}

#endif //!PROFILER_H