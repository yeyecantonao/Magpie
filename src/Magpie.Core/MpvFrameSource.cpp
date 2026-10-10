#include "pch.h"
#include "MpvFrameSource.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"
#include "Logger.h"

namespace Magpie {

struct MpvFrameSource::Impl {
	// libmpv 渲染上下文、D3D11 纹理与命令队列在此落地。
	// 依赖集成方式（Conan / vcpkg / 预编译 SDK）确认后实现。
};

MpvFrameSource::MpvFrameSource() {
	_frameArrivedEvent.create(wil::EventOptions::ManualReset);
}

bool MpvFrameSource::_Initialize() noexcept {
	_impl = std::make_unique<Impl>();

	// 创建视频帧输出纹理（尺寸先取默认，播放文件后按实际分辨率重建）。
	// 由 Renderer 的 shader 管线消费，使用默认绑定方式。
	_output = DirectXHelper::CreateTexture2D(
		_deviceResources->GetD3DDevice(),
		DXGI_FORMAT_B8G8R8A8_UNORM,
		1920, 1080,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		D3D11_USAGE_DEFAULT,
		0
	);
	if (!_output) {
		Logger::Get().Error("创建播放器帧源输出纹理失败");
		return false;
	}

	Logger::Get().Info("MpvFrameSource 初始化完成（播放器模式）");
	return true;
}

FrameSourceState MpvFrameSource::_Update() noexcept {
	_BeginCaptureRender();

	if (!_hasFrame.load(std::memory_order_acquire)) {
		return FrameSourceState::Waiting;
	}

	// TODO(impl): 从 libmpv 渲染上下文取出最新视频帧，写入 _output。
	// 当前为骨架实现：无视频帧数据时维持上一帧。

	_hasFrame.store(false, std::memory_order_release);
	return FrameSourceState::NewFrame;
}

bool MpvFrameSource::OpenFile(const wchar_t* path) noexcept {
	(void)path;
	// TODO(impl): 通过 libmpv 命令队列打开文件，等待首帧解码。
	Logger::Get().Info("MpvFrameSource::OpenFile（骨架）");
	return true;
}

bool MpvFrameSource::Play() noexcept {
	_isPaused.store(false, std::memory_order_release);
	// TODO(impl): 通知 libmpv 播放。
	return true;
}

bool MpvFrameSource::Pause() noexcept {
	_isPaused.store(true, std::memory_order_release);
	// TODO(impl): 通知 libmpv 暂停。
	return true;
}

bool MpvFrameSource::TogglePause() noexcept {
	return _isPaused.load(std::memory_order_acquire) ? Play() : Pause();
}

void MpvFrameSource::Seek(double positionSeconds) noexcept {
	(void)positionSeconds;
	// TODO(impl): libmpv seek。
}

void MpvFrameSource::SetVolume(double volume) noexcept {
	(void)volume;
	// TODO(impl): libmpv volume。
}

bool MpvFrameSource::IsPaused() const noexcept {
	return _isPaused.load(std::memory_order_acquire);
}

double MpvFrameSource::DurationSeconds() const noexcept {
	// TODO(impl): 返回当前播放文件时长。
	return 0.0;
}

double MpvFrameSource::PositionSeconds() const noexcept {
	// TODO(impl): 返回当前播放位置。
	return 0.0;
}

}
