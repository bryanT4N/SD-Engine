#include "Engine/Renderer/SpriteAnimDefinition.hpp"

//-----------------------------------------------------------------------------------------------
SpriteAnimDefinition::SpriteAnimDefinition( SpriteSheet const& spriteSheet, int startSpriteIndex, int endSpriteIndex,
	float framesPerSecond, SpriteAnimPlaybackType playbackType )
	: m_spriteSheet( spriteSheet )
	, m_startSpriteIndex( startSpriteIndex )
	, m_endSpriteIndex( endSpriteIndex )
	, m_framesPerSecond( framesPerSecond )
	, m_playbackType( playbackType )
{
}

//-----------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	if( m_framesPerSecond <= 0.0f ) {
		return m_spriteSheet.GetSpriteDef( m_startSpriteIndex );
	}

	if (seconds < 0.0f) {
		seconds = 0.0f;
	}

	int frameCount = m_endSpriteIndex - m_startSpriteIndex + 1;
	if (frameCount <= 0) {
		return m_spriteSheet.GetSpriteDef( m_startSpriteIndex );
	}

	float totalFramesFloat = seconds * m_framesPerSecond;
	if( totalFramesFloat < 0.0f ) {
		totalFramesFloat = 0.0f;
	}

	int frameIndex = static_cast<int>( totalFramesFloat );
	int localFrameIndex = 0;

	if( m_playbackType == SpriteAnimPlaybackType::ONCE ) {
		if( frameIndex < 0 ) {
			frameIndex = 0;
		}
		if( frameIndex >= frameCount ) {
			frameIndex = frameCount - 1;
		}
		localFrameIndex = frameIndex;
	}
	else if( m_playbackType == SpriteAnimPlaybackType::LOOP ) {
		if( frameCount > 0 ) {
			int cycleIndex = frameIndex % frameCount;
			if( cycleIndex < 0 ) {
				cycleIndex += frameCount;
			}
			localFrameIndex = cycleIndex;
		}
	}
	else if( m_playbackType == SpriteAnimPlaybackType::PINGPONG ) {
		if( frameCount == 1 ) {
			localFrameIndex = 0;
		}
		else {
			int pingPongLength = frameCount * 2 - 2;
			int cycleIndex = frameIndex % pingPongLength;
			if( cycleIndex < 0 ) {
				cycleIndex += pingPongLength;
			}

			if( cycleIndex < frameCount ) {
				localFrameIndex = cycleIndex;
			}
			else {
				localFrameIndex = pingPongLength - cycleIndex;
			}
		}
	}

	int spriteIndex = m_startSpriteIndex + localFrameIndex;
	return m_spriteSheet.GetSpriteDef( spriteIndex );
}


