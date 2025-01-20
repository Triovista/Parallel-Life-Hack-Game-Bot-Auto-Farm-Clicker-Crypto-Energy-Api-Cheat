
#pragma once

#include "GUObject.h"

namespace CoreStructures {

	typedef __int64 gu_time_index;
	typedef __int64 gu_time_interval;
	typedef double gu_seconds;

	class GUFrameCounter;

	class GUClock : public GUObject {

	private:

		__int64					performanceFrequency;
		gu_seconds				timeRecip;

		gu_time_index			baseTime;
		gu_time_index			prevTimeIndex;
		gu_time_index			currentTimeIndex;

		gu_time_interval		deltaTime;

		gu_time_index			stopTimeIndex;
		gu_time_interval		totalStopTime;

		bool					_clockStopped;

		GUFrameCounter			*frameCounter;


		//
		// Private API
		//

		void resetClockAttributes();
		void invalidateClock();
		gu_seconds convertTimeIntervalToSeconds(gu_time_interval t) const;


	public:

		// Class methods

		static gu_time_index actualTime();


		// Instance methods

		GUClock(void);
		~GUClock(void);


		// clock update methods
		
		void start();
		void stop();
		void tick();
		void reset();


		// clock query methods

		gu_seconds actualTimeElapsed() const;
		gu_seconds gameTimeElapsed() const;
		gu_seconds gameTimeDelta() const;

		bool clockStopped() const;

		void reportTimingData() const;

		int framesPerSecond() const;
		int minimumFPS() const;
		int maximumFPS() const;
		gu_seconds averageFPS() const;
		gu_seconds secondsPerFrame() const;
		gu_seconds minimumSPF() const;
		gu_seconds maximumSPF() const;
		gu_seconds averageSPF() const;

	};

}
