#ifndef ANIM_CLIP_H
#define ANIM_CLIP_H

#include <map>
#include <vector>

#include "Math/Quaternion.h"
#include "Math/Vector.h"
#include "Resource/Resource.h"

namespace PizzaBox{
	template <class T>
	struct KeyFrame{
		KeyFrame(float time_, const T& value_) : time(time_), value(value_){}

		float time;
		T value;
	};

	using PosKeyFrame = KeyFrame<Vector3>;
	using RotKeyFrame = KeyFrame<Quaternion>;
	using ScaleKeyFrame = KeyFrame<Vector3>;

	class AnimClip : public Resource{
	public:
		explicit AnimClip(const std::string& filePath_);
		virtual ~AnimClip() override;

		virtual bool Load() override;
		virtual void Unload() override;

		inline float GetLength() const{ return length; }
		inline void SetLength(float length_){ length = length_; }
		void AddPosKey(const std::string& name_, const PosKeyFrame& keyFrame_);
		void AddRotKey(const std::string& name_, const RotKeyFrame& keyFrame_);
		void AddScaleKey(const std::string& name_, const ScaleKeyFrame& keyFrame_);

		bool HasKeysForJoint(const std::string& name_) const;
		Matrix4 GetTransformAtTime(const std::string& jointName_, float time_) const;

		Vector3 GetTranslateAtTime(const std::string& jointName_, float time_) const;
		Quaternion GetRotateAtTime(const std::string& jointName_, float time_) const;
		Vector3 GetScaleAtTime(const std::string& jointName_, float time_) const;

	private:
		float length;
		std::map<std::string, std::vector<PosKeyFrame>> posKeys;
		std::map<std::string, std::vector<RotKeyFrame>> rotKeys;
		std::map<std::string, std::vector<ScaleKeyFrame>> scaleKeys;

		size_t FindPositionKey(const std::string& jointName_, float time) const;
		size_t FindRotationKey(const std::string& jointName_, float time) const;
		size_t FindScaleKey(const std::string& jointName_, float time) const;

		Vector3 CalculateTranslation(const PosKeyFrame& prevKey_, const PosKeyFrame& nextKey_, float time_) const;
		Quaternion CalculateRotation(const RotKeyFrame& prevKey_, const RotKeyFrame& nextKey_, float time_) const;
		Vector3 CalculateScale(const ScaleKeyFrame& prevKey_, const ScaleKeyFrame& nextKey_, float time_) const;

		template <class T>
		size_t ForwardSearch(const std::vector<T>& keyFrames_, float time_) const{
			for(size_t i = 0; i < keyFrames_.size() - 1; i++){
				if(time_ < keyFrames_[i + 1].time){
					return i;
				}
			}

			return keyFrames_.size() - 2;
		}

		template <class T>
		size_t BackSearch(const std::vector<T>& keyFrames_, float time_) const{
			for(size_t i = keyFrames_.size() - 1; i > 0; i--){
				if(time_ > keyFrames_[i - 1].time){
					return i - 1;
				}
			}

			return 0;
		}
	};
}

#endif //!ANIM_CLIP_H