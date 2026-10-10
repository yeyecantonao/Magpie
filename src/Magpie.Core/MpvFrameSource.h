#pragma once
#include "FrameSourceBase.h"
#include <winrt/Windows.Storage.h>

namespace Magpie {

// 基于 libmpv 的视频帧源。将解码后的视频帧渲染进 D3D11 纹理，
// 复用 Magpie 的缩放/增强渲染管线。
// 播放控制（打开/播放/暂停/进度/音量）由宿主播放器 UI 通过本类接口驱动。
class MpvFrameSource : public FrameSourceBase {
public:
	MpvFrameSource();

	// 播放器控制接口（由 UI 线程调用）
	bool OpenFile(const wchar_t* path) noexcept;
	bool Play() noexcept;
	bool Pause() noexcept;
	bool TogglePause() noexcept;
	void Seek(double positionSeconds) noexcept;
	void SetVolume(double volume) noexcept;   // 0.0 ~ 1.0
	bool IsPaused() const noexcept;
	double DurationSeconds() const noexcept;
	double PositionSeconds() const noexcept;

	const char* Name() const noexcept override {
		return "MpvFrameSource";
	}

	FrameSourceWaitType WaitType() const noexcept override {
		return FrameSourceWaitType::WaitForEvent;
	}

	// 视频帧到达事件（后端线程等待它驱动渲染）
	HANDLE FrameArrivedEvent() const noexcept override {
		return _frameArrivedEvent.get();
	}

protected:
	bool _Initialize() noexcept override;
	FrameSourceState _Update() noexcept override;

private:
	// libmpv 渲染上下文与命令队列（实现随依赖集成方式落地）
	struct Impl;
	std::unique_ptr<Impl> _impl;

	wil::unique_event_nothrow _frameArrivedEvent;
	std::atomic<bool> _hasFrame{ false };
	std::atomic<bool> _isPaused{ true };
};

}
