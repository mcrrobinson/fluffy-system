#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>

HMIDIOUT hMidiOut;

// Function to send a MIDI note-on message
void noteOn(HMIDIOUT hMidiOut, BYTE note, BYTE velocity)
{
    DWORD msg = 0x90 | ((DWORD)note << 8) | ((DWORD)velocity << 16);
    midiOutShortMsg(hMidiOut, msg);
}

// Function to send a MIDI note-off message
void noteOff(HMIDIOUT hMidiOut, BYTE note)
{
    DWORD msg = 0x80 | ((DWORD)note << 8);
    midiOutShortMsg(hMidiOut, msg);
}

// MIDI callback function
void CALLBACK MidiInCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (wMsg == MIM_DATA)
    {
        BYTE status = (BYTE)dwParam1;
        BYTE data1 = (BYTE)(dwParam1 >> 8);
        BYTE data2 = (BYTE)(dwParam1 >> 16);

        // Check if it's a Note On event (status: 0x90) and the velocity is greater than 0 (note-on)
        if ((status & 0xF0) == 0x90 && data2 > 0)
        {
            printf("Note On - Note: %d, Velocity: %d\n", (int)data1, (int)data2);

            // Play the received note with the same velocity
            noteOn(hMidiOut, data1, data2);
        }
        // Check if it's a Note Off event (status: 0x80) or Note On with velocity 0 (note-off)
        else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0))
        {
            printf("Note Off - Note: %d\n", (int)data1);

            // Stop playing the received note
            noteOff(hMidiOut, data1);
        }
    }
}

int main()
{

    if (midiOutOpen(&hMidiOut, MIDI_MAPPER, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
    {
        printf("Error opening MIDI output.\n");
        return 1;
    }

    HMIDIIN hMidiIn;
    UINT numDevices = midiInGetNumDevs();

    if (numDevices == 0)
    {
        printf("No MIDI input devices available.\n");
        return 1;
    }

    // Open the first available MIDI input device
    if (midiInOpen(&hMidiIn, 0, (DWORD_PTR)MidiInCallback, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
    {
        printf("Error opening MIDI input.\n");
        return 1;
    }

    // Start MIDI input
    if (midiInStart(hMidiIn) != MMSYSERR_NOERROR)
    {
        printf("Error starting MIDI input.\n");
        return 1;
    }

    printf("Listening for MIDI input from your piano keyboard. Press Enter to exit...\n");
    while (getchar() != '\n')
        ; // Wait for Enter key press

    // Stop and close MIDI input
    midiInStop(hMidiIn);
    midiInClose(hMidiIn);

    // Close the MIDI output device
    midiOutClose(hMidiOut);

    return 0;
}