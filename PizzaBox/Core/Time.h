#ifndef TIME_H
#define TIME_H

namespace PizzaBox{
	class Time{
	public:
		static bool Initialize();
		static void Destroy();

		static void Start();
		static float DeltaTime();
		static void UpdateFrameTicks();
		static float RealDeltaTime();
		static float PureDeltaTime();
		static void Delay();
		static void SetFrameRate(const unsigned int fps);
		static void SetTimeScale(const float scale);

		static double TimeSinceStartup();

		//Delete unwanted compiler generated constructors, destructors and assignment operators
		Time() = delete;
		Time(const Time&) = delete;
		Time(Time&&) = delete;
		Time& operator=(const Time&) = delete;
		Time& operator=(Time&&) = delete;
		~Time() = delete;

	private:
		static unsigned int frameRate;
		static float timeScale;
		static unsigned int startTime;
		static unsigned int prevTicks;
		static unsigned int currTicks;

		static unsigned int GetSleepTime();
	};
}

#endif //!TIME_H