using System;

namespace Vault
{
    public class Audio2D
    {
        public static void PlaySound(string path, float volume = 1.0f, bool loop = false, int channel = -1)
        {
            InternalCalls.Audio2D_PlaySound(path, volume, loop, channel);
        }

        public static void PlayMusic(string path, float volume = 1.0f, bool loop = false)
        {
            InternalCalls.Audio2D_PlayMusic(path, volume, loop);
        }

        public static void StopSound(int channel = -1)
        {
            InternalCalls.Audio2D_StopSound(channel);
        }

        public static void StopMusic()
        {
            InternalCalls.Audio2D_StopMusic();
        }
    }
}