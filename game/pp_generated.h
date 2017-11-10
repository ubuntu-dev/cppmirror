#if !defined(PP_GENERATED_H)

#define PP_IGNORE // TODO(Jonny): Put this before a #include to ignore the file.

#include <stdint.h>
#include <string.h>

#if !defined(PP_ASSERT)
    #include <assert.h>
    #define PP_ASSERT assert
#endif

#if !defined(PP_SPRINTF)
    #include "stdio.h" 
    #if defined(_MSC_VER)
        #define PP_SPRINTF(buf, size, format, ...) sprintf_s(buf, size, format, ##__VA_ARGS__)
    #else
        #define PP_SPRINTF(buf, size, format, ...) sprintf(buf, format, ##__VA_ARGS__)
    #endif
#endif

#define PP_CONCAT(a, b) a##b
#define PP_TO_STRING(a) #a

#define PP_OFFSETOF(T, member) ((uintptr_t)&(((T *)0)->member))

#if defined(PP_STATIC_FUNCS)
    #define PP_STATIC static
#else
    #define PP_STATIC
#endif

#define PP_TRUE 1
#define PP_FALSE 0

typedef struct pp___m128 { float e[4]; } pp___m128;
typedef struct pp___m128i { int e[4]; } pp___m128i;

// Primitive types.
typedef void pp_void;
typedef int32_t pp_MyBool;typedef char pp_char;
typedef short pp_short;
typedef int pp_int;
typedef long pp_long;
typedef float pp_float;
typedef double pp_double;
typedef uint64_t pp_uint64_t;
typedef uint32_t pp_uint32_t;
typedef uint16_t pp_uint16_t;
typedef uint8_t pp_uint8_t;
typedef int64_t pp_int64_t;
typedef int32_t pp_int32_t;
typedef int16_t pp_int16_t;
typedef int8_t pp_int8_t;
typedef uintptr_t pp_uintptr_t;
typedef intptr_t pp_intptr_t;
typedef size_t pp_size_t;

PP_STATIC pp_MyBool pp_string_compare(char const *a, char const *b) {
    for(; (*a != *b); ++a, ++b) {
        if(!(*a) && !(*b)) {
            return(PP_TRUE);
        }
    }

    return(PP_TRUE);
}

#if !defined(PP_MEMSET)
PP_STATIC void *PP_MEMSET(void *dst, uint8_t v, uintptr_t size) {
    uint8_t *dst8 = (uint8_t *)dst;
    while(size-- > 0) {
        *dst8++ = v;
    }

    return(dst);
}
#endif
//
// Forward declared structs, enums, and functions
//
typedef enum sglp_Key sglp_Key;
typedef enum Player_Direction Player_Direction;
typedef enum ID ID;
typedef struct sglp_Sprite sglp_Sprite;
typedef struct sglp_PlayingSound sglp_PlayingSound;
typedef struct sglp_OpenGlFunctions sglp_OpenGlFunctions;
typedef struct sglp_Settings sglp_Settings;
typedef struct sglp_File sglp_File;
typedef struct sglp_API sglp_API;
typedef struct sglp_LoadedSound sglp_LoadedSound;
typedef struct sglp_SoundOutputBuffer sglp_SoundOutputBuffer;
typedef struct sglp_AudioState sglp_AudioState;
typedef struct sglp_WAVEHeader sglp_WAVEHeader;
typedef struct sglp_WavChunk sglp_WavChunk;
typedef struct sglp_WavFormat sglp_WavFormat;
typedef struct sglp_RiffIter sglp_RiffIter;
typedef struct SGLP_XINPUT_GAMEPAD SGLP_XINPUT_GAMEPAD;
typedef struct SGLP_XINPUT_STATE SGLP_XINPUT_STATE;
typedef struct SGLP_XINPUT_VIBRATION SGLP_XINPUT_VIBRATION;
typedef struct sglp_Win32SoundOutput sglp_Win32SoundOutput;
typedef struct WorkQueueEntry WorkQueueEntry;
typedef struct WorkQueue WorkQueue;
typedef union sglm_V2 sglm_V2;
typedef struct sglm_Mat4x4 sglm_Mat4x4;
typedef struct Entity Entity;
typedef struct Player Player;
typedef struct Game_State Game_State;
typedef struct V2 V2;

//
// An enum, with an index for each type in the codebase.
//
typedef enum pp_Type {
    pp_Type_unknown,
    pp_Type_char,
    pp_Type_short,
    pp_Type_int,
    pp_Type_long,
    pp_Type_float,
    pp_Type_double,
    pp_Type_uint64_t,
    pp_Type_uint32_t,
    pp_Type_uint16_t,
    pp_Type_uint8_t,
    pp_Type_int64_t,
    pp_Type_int32_t,
    pp_Type_int16_t,
    pp_Type_int8_t,
    pp_Type_uintptr_t,
    pp_Type_intptr_t,
    pp_Type_size_t,
    pp_Type_sglp_Bool,
    pp_Type_sglp_GLenum,
    pp_Type_sglp_GLbitfield,
    pp_Type_sglp_GLuint,
    pp_Type_sglp_GLint,
    pp_Type_sglp_GLsizei,
    pp_Type_sglp_GLboolean,
    pp_Type_sglp_GLbyte,
    pp_Type_sglp_GLshort,
    pp_Type_sglp_GLubyte,
    pp_Type_sglp_GLushort,
    pp_Type_sglp_GLulong,
    pp_Type_sglp_GLfloat,
    pp_Type_sglp_GLclampf,
    pp_Type_sglp_GLdouble,
    pp_Type_sglp_GLclampd,
    pp_Type_sglp_GLvoid,
    pp_Type_sglp_GLchar,
    pp_Type_sglp_GLsizeiptr,
    pp_Type_sglp_glBindTexture_t,
    pp_Type_sglp_glClear_t,
    pp_Type_sglp_glClearColor_t,
    pp_Type_sglp_glDrawArrays_t,
    pp_Type_sglp_glGetError_t,
    pp_Type_sglp_glTexImage2D_t,
    pp_Type_sglp_glTexParameteri_t,
    pp_Type_sglp_glGetString_t,
    pp_Type_sglp_glViewport_t,
    pp_Type_sglp_glCreateShader_t,
    pp_Type_sglp_glShaderSource_t,
    pp_Type_sglp_glCompileShader_t,
    pp_Type_sglp_glGetShaderiv_t,
    pp_Type_sglp_glCreateProgram_t,
    pp_Type_sglp_glAttachShader_t,
    pp_Type_sglp_glBindAttribLocation_t,
    pp_Type_sglp_glLinkProgram_t,
    pp_Type_sglp_glUseProgram_t,
    pp_Type_sglp_glGenBuffers_t,
    pp_Type_sglp_glBindBuffer_t,
    pp_Type_sglp_glBufferData_t,
    pp_Type_sglp_glVertexAttribPointer_t,
    pp_Type_sglp_glEnableVertexAttribArray_t,
    pp_Type_sglp_glUniform1i_t,
    pp_Type_sglp_glUniform2f_t,
    pp_Type_sglp_glUniform4f_t,
    pp_Type_sglp_glUniformMatrix4fv_t,
    pp_Type_sglp_glGetUniformLocation_t,
    pp_Type_sglp_glIsShader_t,
    pp_Type_sglp_glGetProgramiv_t,
    pp_Type_sglp_glGetProgramInfoLog_t,
    pp_Type_sglp_glGetShaderInfoLog_t,
    pp_Type_sglp_glGenVertexArrays_t,
    pp_Type_sglp_glBindVertexArray_t,
    pp_Type_sglp_TranslateMessage_t,
    pp_Type_sglp_DispatchMessageA_t,
    pp_Type_sglp_PeekMessageA_t,
    pp_Type_sglp_DefWindowProcA_t,
    pp_Type_sglp_RegisterClassA_t,
    pp_Type_sglp_CreateWindowExA_t,
    pp_Type_sglp_SetWindowPos_t,
    pp_Type_sglp_GetWindowPlacement_t,
    pp_Type_sglp_SetWindowPlacement_t,
    pp_Type_sglp_GetDC_t,
    pp_Type_sglp_GetCursorPos_t,
    pp_Type_sglp_ScreenToClient_t,
    pp_Type_sglp_GetWindowLongA_t,
    pp_Type_sglp_SetWindowLongA_t,
    pp_Type_sglp_MonitorFromWindow_t,
    pp_Type_sglp_GetMonitorInfoA_t,
    pp_Type_sglp_GetSystemMetrics_t,
    pp_Type_sglp_ChoosePixelFormat_t,
    pp_Type_sglp_DescribePixelFormat_t,
    pp_Type_sglp_SetPixelFormat_t,
    pp_Type_sglp_SwapBuffers_t,
    pp_Type_DirectSoundCreate,
    pp_Type_sglp_XInputGetState_t,
    pp_Type_sglp_XInputSetState_t,
    pp_Type_wglGetProcAddress_t,
    pp_Type_wglCreateContext_t,
    pp_Type_wglMakeCurrent_t,
    pp_Type_wglDeleteContext_t,
    pp_Type_wglCreateContextAttribsArb_t,
    pp_Type_wglSwapIntervalExt_t,
    pp_Type_TimeBeginPeriod_t,
    pp_Type_sglp_XOpenDisplay_t,
    pp_Type_sglp_XCreateColorMap_t,
    pp_Type_sglp_XCreateWindow_t,
    pp_Type_sglp_XMapWindow_t,
    pp_Type_sglp_XStoreName_t,
    pp_Type_sglp_XSelectInput_t,
    pp_Type_sglp_XPending_t,
    pp_Type_sglp_XNextEvent_t,
    pp_Type_sglp_XKeycodeToKeysym_t,
    pp_Type_sglp_XGetWindowAttributes_t,
    pp_Type_sglp_XQueryPointer_t,
    pp_Type_sglp_sem_open_t,
    pp_Type_sglp_sem_close_t,
    pp_Type_sglp_pthread_create_t,
    pp_Type_sglp_glXGetProcAddressArb_t,
    pp_Type_sglp_glXChooseVisual_t,
    pp_Type_sglp_glXCreateContext_t,
    pp_Type_sglp_glXMakeCurrent_t,
    pp_Type_sglp_glXSwapBuffers_t,
    pp_Type_sglm_Bool,
    pp_Type_sglp_Key,
    pp_Type_Player_Direction,
    pp_Type_ID,
    pp_Type_sglp_Sprite,
    pp_Type_sglp_PlayingSound,
    pp_Type_sglp_OpenGlFunctions,
    pp_Type_sglp_Settings,
    pp_Type_sglp_File,
    pp_Type_sglp_API,
    pp_Type_void,
    pp_Type_sglp_LoadedSound,
    pp_Type_sglp_SoundOutputBuffer,
    pp_Type_sglp_AudioState,
    pp_Type_sglp_WAVEHeader,
    pp_Type_sglp_WavChunk,
    pp_Type_sglp_WavFormat,
    pp_Type_sglp_RiffIter,
    pp_Type_SGLP_XINPUT_GAMEPAD,
    pp_Type_SGLP_XINPUT_STATE,
    pp_Type_SGLP_XINPUT_VIBRATION,
    pp_Type_sglp_Win32SoundOutput,
    pp_Type_WorkQueueEntry,
    pp_Type_WorkQueue,
    pp_Type_sglm_V2,
    pp_Type_sglm_Mat4x4,
    pp_Type___m128,
    pp_Type_Entity,
    pp_Type_Player,
    pp_Type_Game_State,
    pp_Type_V2,
    pp_Type___m128i,

    pp_Type_count
} pp_Type;

//
// Forward declared recreated stuff.
//

// Forward declared structs.
typedef struct pp_sglp_Sprite pp_sglp_Sprite;    typedef struct pp_sglp_Sprite pp_pp_sglp_Sprite;
typedef struct pp_sglp_PlayingSound pp_sglp_PlayingSound;    typedef struct pp_sglp_PlayingSound pp_pp_sglp_PlayingSound;
typedef struct pp_sglp_OpenGlFunctions pp_sglp_OpenGlFunctions;    typedef struct pp_sglp_OpenGlFunctions pp_pp_sglp_OpenGlFunctions;
typedef struct pp_sglp_Settings pp_sglp_Settings;    typedef struct pp_sglp_Settings pp_pp_sglp_Settings;
typedef struct pp_sglp_File pp_sglp_File;    typedef struct pp_sglp_File pp_pp_sglp_File;
typedef struct pp_sglp_API pp_sglp_API;    typedef struct pp_sglp_API pp_pp_sglp_API;
typedef struct pp_sglp_LoadedSound pp_sglp_LoadedSound;    typedef struct pp_sglp_LoadedSound pp_pp_sglp_LoadedSound;
typedef struct pp_sglp_SoundOutputBuffer pp_sglp_SoundOutputBuffer;    typedef struct pp_sglp_SoundOutputBuffer pp_pp_sglp_SoundOutputBuffer;
typedef struct pp_sglp_AudioState pp_sglp_AudioState;    typedef struct pp_sglp_AudioState pp_pp_sglp_AudioState;
typedef struct pp_sglp_WAVEHeader pp_sglp_WAVEHeader;    typedef struct pp_sglp_WAVEHeader pp_pp_sglp_WAVEHeader;
typedef struct pp_sglp_WavChunk pp_sglp_WavChunk;    typedef struct pp_sglp_WavChunk pp_pp_sglp_WavChunk;
typedef struct pp_sglp_WavFormat pp_sglp_WavFormat;    typedef struct pp_sglp_WavFormat pp_pp_sglp_WavFormat;
typedef struct pp_sglp_RiffIter pp_sglp_RiffIter;    typedef struct pp_sglp_RiffIter pp_pp_sglp_RiffIter;
typedef struct pp_SGLP_XINPUT_GAMEPAD pp_SGLP_XINPUT_GAMEPAD;    typedef struct pp_SGLP_XINPUT_GAMEPAD pp_pp_SGLP_XINPUT_GAMEPAD;
typedef struct pp_SGLP_XINPUT_STATE pp_SGLP_XINPUT_STATE;    typedef struct pp_SGLP_XINPUT_STATE pp_pp_SGLP_XINPUT_STATE;
typedef struct pp_SGLP_XINPUT_VIBRATION pp_SGLP_XINPUT_VIBRATION;    typedef struct pp_SGLP_XINPUT_VIBRATION pp_pp_SGLP_XINPUT_VIBRATION;
typedef struct pp_sglp_Win32SoundOutput pp_sglp_Win32SoundOutput;    typedef struct pp_sglp_Win32SoundOutput pp_pp_sglp_Win32SoundOutput;
typedef struct pp_WorkQueueEntry pp_WorkQueueEntry;    typedef struct pp_WorkQueueEntry pp_pp_WorkQueueEntry;
typedef struct pp_WorkQueue pp_WorkQueue;    typedef struct pp_WorkQueue pp_pp_WorkQueue;
typedef union pp_sglm_V2 pp_sglm_V2;    typedef union pp_sglm_V2 pp_pp_sglm_V2;
typedef struct pp_sglm_Mat4x4 pp_sglm_Mat4x4;    typedef struct pp_sglm_Mat4x4 pp_pp_sglm_Mat4x4;
typedef struct pp_Entity pp_Entity;    typedef struct pp_Entity pp_pp_Entity;
typedef struct pp_Player pp_Player;    typedef struct pp_Player pp_pp_Player;
typedef struct pp_Game_State pp_Game_State;    typedef struct pp_Game_State pp_pp_Game_State;
typedef struct pp_V2 pp_V2;    typedef struct pp_V2 pp_pp_V2;

// Forward declared enums
typedef int pp_sglp_Key;
typedef int pp_Player_Direction;
typedef int pp_ID;

//
// Create typedefs.
//
typedef pp_int pp_sglp_Bool;
typedef pp_uint32_t pp_sglp_GLenum;
typedef pp_uint32_t pp_sglp_GLbitfield;
typedef pp_uint32_t pp_sglp_GLuint;
typedef pp_int pp_sglp_GLint;
typedef pp_int pp_sglp_GLsizei;
typedef pp_uint8_t pp_sglp_GLboolean;
typedef pp_char pp_sglp_GLbyte;
typedef pp_short pp_sglp_GLshort;
typedef pp_uint8_t pp_sglp_GLubyte;
typedef pp_uint16_t pp_sglp_GLushort;
typedef pp_uint32_t pp_sglp_GLulong;
typedef pp_float pp_sglp_GLfloat;
typedef pp_float pp_sglp_GLclampf;
typedef pp_double pp_sglp_GLdouble;
typedef pp_double pp_sglp_GLclampd;
typedef pp_void pp_sglp_GLvoid;
typedef pp_char pp_sglp_GLchar;
typedef pp_long pp_sglp_GLsizeiptr;
typedef pp_uintptr_t pp_sglp_glBindTexture_t;
typedef pp_uintptr_t pp_sglp_glClear_t;
typedef pp_uintptr_t pp_sglp_glClearColor_t;
typedef pp_uintptr_t pp_sglp_glDrawArrays_t;
typedef pp_uintptr_t pp_sglp_glGetError_t;
typedef pp_uintptr_t pp_sglp_glTexImage2D_t;
typedef pp_uintptr_t pp_sglp_glTexParameteri_t;
typedef pp_uintptr_t pp_sglp_glGetString_t;
typedef pp_uintptr_t pp_sglp_glViewport_t;
typedef pp_uintptr_t pp_sglp_glCreateShader_t;
typedef pp_uintptr_t pp_sglp_glShaderSource_t;
typedef pp_uintptr_t pp_sglp_glCompileShader_t;
typedef pp_uintptr_t pp_sglp_glGetShaderiv_t;
typedef pp_uintptr_t pp_sglp_glCreateProgram_t;
typedef pp_uintptr_t pp_sglp_glAttachShader_t;
typedef pp_uintptr_t pp_sglp_glBindAttribLocation_t;
typedef pp_uintptr_t pp_sglp_glLinkProgram_t;
typedef pp_uintptr_t pp_sglp_glUseProgram_t;
typedef pp_uintptr_t pp_sglp_glGenBuffers_t;
typedef pp_uintptr_t pp_sglp_glBindBuffer_t;
typedef pp_uintptr_t pp_sglp_glBufferData_t;
typedef pp_uintptr_t pp_sglp_glVertexAttribPointer_t;
typedef pp_uintptr_t pp_sglp_glEnableVertexAttribArray_t;
typedef pp_uintptr_t pp_sglp_glUniform1i_t;
typedef pp_uintptr_t pp_sglp_glUniform2f_t;
typedef pp_uintptr_t pp_sglp_glUniform4f_t;
typedef pp_uintptr_t pp_sglp_glUniformMatrix4fv_t;
typedef pp_uintptr_t pp_sglp_glGetUniformLocation_t;
typedef pp_uintptr_t pp_sglp_glIsShader_t;
typedef pp_uintptr_t pp_sglp_glGetProgramiv_t;
typedef pp_uintptr_t pp_sglp_glGetProgramInfoLog_t;
typedef pp_uintptr_t pp_sglp_glGetShaderInfoLog_t;
typedef pp_uintptr_t pp_sglp_glGenVertexArrays_t;
typedef pp_uintptr_t pp_sglp_glBindVertexArray_t;
typedef pp_uintptr_t pp_sglp_TranslateMessage_t;
typedef pp_uintptr_t pp_sglp_DispatchMessageA_t;
typedef pp_uintptr_t pp_sglp_PeekMessageA_t;
typedef pp_uintptr_t pp_sglp_DefWindowProcA_t;
typedef pp_uintptr_t pp_sglp_RegisterClassA_t;
typedef pp_uintptr_t pp_sglp_CreateWindowExA_t;
typedef pp_uintptr_t pp_sglp_SetWindowPos_t;
typedef pp_uintptr_t pp_sglp_GetWindowPlacement_t;
typedef pp_uintptr_t pp_sglp_SetWindowPlacement_t;
typedef pp_uintptr_t pp_sglp_GetDC_t;
typedef pp_uintptr_t pp_sglp_GetCursorPos_t;
typedef pp_uintptr_t pp_sglp_ScreenToClient_t;
typedef pp_uintptr_t pp_sglp_GetWindowLongA_t;
typedef pp_uintptr_t pp_sglp_SetWindowLongA_t;
typedef pp_uintptr_t pp_sglp_MonitorFromWindow_t;
typedef pp_uintptr_t pp_sglp_GetMonitorInfoA_t;
typedef pp_uintptr_t pp_sglp_GetSystemMetrics_t;
typedef pp_uintptr_t pp_sglp_ChoosePixelFormat_t;
typedef pp_uintptr_t pp_sglp_DescribePixelFormat_t;
typedef pp_uintptr_t pp_sglp_SetPixelFormat_t;
typedef pp_uintptr_t pp_sglp_SwapBuffers_t;
typedef pp_uintptr_t pp_DirectSoundCreate;
typedef pp_uintptr_t pp_sglp_XInputGetState_t;
typedef pp_uintptr_t pp_sglp_XInputSetState_t;
typedef pp_uintptr_t pp_wglGetProcAddress_t;
typedef pp_uintptr_t pp_wglCreateContext_t;
typedef pp_uintptr_t pp_wglMakeCurrent_t;
typedef pp_uintptr_t pp_wglDeleteContext_t;
typedef pp_uintptr_t pp_wglCreateContextAttribsArb_t;
typedef pp_uintptr_t pp_wglSwapIntervalExt_t;
typedef pp_uintptr_t pp_TimeBeginPeriod_t;
typedef pp_uintptr_t pp_sglp_XOpenDisplay_t;
typedef pp_uintptr_t pp_sglp_XCreateColorMap_t;
typedef pp_uintptr_t pp_sglp_XCreateWindow_t;
typedef pp_uintptr_t pp_sglp_XMapWindow_t;
typedef pp_uintptr_t pp_sglp_XStoreName_t;
typedef pp_uintptr_t pp_sglp_XSelectInput_t;
typedef pp_uintptr_t pp_sglp_XPending_t;
typedef pp_uintptr_t pp_sglp_XNextEvent_t;
typedef pp_uintptr_t pp_sglp_XKeycodeToKeysym_t;
typedef pp_uintptr_t pp_sglp_XGetWindowAttributes_t;
typedef pp_uintptr_t pp_sglp_XQueryPointer_t;
typedef pp_uintptr_t pp_sglp_sem_open_t;
typedef pp_uintptr_t pp_sglp_sem_close_t;
typedef pp_uintptr_t pp_sglp_pthread_create_t;
typedef pp_uintptr_t pp_sglp_glXGetProcAddressArb_t;
typedef pp_uintptr_t pp_sglp_glXChooseVisual_t;
typedef pp_uintptr_t pp_sglp_glXCreateContext_t;
typedef pp_uintptr_t pp_sglp_glXMakeCurrent_t;
typedef pp_uintptr_t pp_sglp_glXSwapBuffers_t;
typedef pp_int pp_sglm_Bool;

//
// Recreated structs.
//
struct pp_sglp_Sprite {
    int32_t w; int32_t h; int32_t id; int32_t frame_cnt_x; int32_t frame_cnt_y; uint32_t vbo_arr[2]; uint32_t mesh; 
};
struct pp_sglp_PlayingSound {
    pp_sglp_PlayingSound *next; float cur_volume0; float cur_volume1; float target_volume0; float target_volume1; float dcur_volume0; float dcur_volume1; float dsample; int32_t id; float samples_played; 
};
struct pp_sglp_OpenGlFunctions {
    pp_sglp_glBindTexture_t *glBindTexture; pp_sglp_glClear_t *glClear; pp_sglp_glClearColor_t *glClearColor; pp_sglp_glDrawArrays_t *glDrawArrays; pp_sglp_glGetError_t *glGetError; pp_sglp_glTexImage2D_t *glTexImage2D; pp_sglp_glTexParameteri_t *glTexParameteri; pp_sglp_glGetString_t *glGetString; pp_sglp_glViewport_t *glViewport; pp_sglp_glCreateShader_t *glCreateShader; pp_sglp_glShaderSource_t *glShaderSource; pp_sglp_glCompileShader_t *glCompileShader; pp_sglp_glGetShaderiv_t *glGetShaderiv; pp_sglp_glCreateProgram_t *glCreateProgram; pp_sglp_glAttachShader_t *glAttachShader; pp_sglp_glBindAttribLocation_t *glBindAttribLocation; pp_sglp_glLinkProgram_t *glLinkProgram; pp_sglp_glUseProgram_t *glUseProgram; pp_sglp_glGenBuffers_t *glGenBuffers; pp_sglp_glBindBuffer_t *glBindBuffer; pp_sglp_glBufferData_t *glBufferData; pp_sglp_glVertexAttribPointer_t *glVertexAttribPointer; pp_sglp_glEnableVertexAttribArray_t *glEnableVertexAttribArray; pp_sglp_glUniform1i_t *glUniform1i; pp_sglp_glUniform2f_t *glUniform2f; pp_sglp_glUniform4f_t *glUniform4f; pp_sglp_glUniformMatrix4fv_t *glUniformMatrix4fv; pp_sglp_glGetUniformLocation_t *glGetUniformLocation; pp_sglp_glIsShader_t *glIsShader; pp_sglp_glGetProgramiv_t *glGetProgramiv; pp_sglp_glGetProgramInfoLog_t *glGetProgramInfoLog; pp_sglp_glGetShaderInfoLog_t *glGetShaderInfoLog; pp_sglp_glGenVertexArrays_t *glGenVertexArrays; pp_sglp_glBindVertexArray_t *glBindVertexArray; 
};
struct pp_sglp_Settings {
    pp_sglp_Bool fullscreen; int32_t win_width; int32_t win_height; int32_t frame_rate; uintptr_t permanent_memory_size; int32_t max_no_of_sounds; char const *window_title; int32_t thread_cnt; 
};
struct pp_sglp_File {
    uint8_t *e; uintptr_t size; 
};
struct pp_sglp_API {
    pp_sglp_Settings settings; pp_sglp_OpenGlFunctions gl; pp_void *permanent_memory; float key[256]; float dt; pp_sglp_Bool init_game; pp_sglp_Bool quit; 
};
struct pp_sglp_LoadedSound {
    uint32_t sample_cnt; uint32_t no_channels; int16_t *samples[2]; int32_t id; 
};
struct pp_sglp_SoundOutputBuffer {
    int16_t *samples; int32_t samples_per_second; int32_t sample_cnt; 
};
struct pp_sglp_AudioState {
    pp_sglp_PlayingSound *first_playing_snd; pp_sglp_PlayingSound *first_free_playing_snd; 
};
struct pp_sglp_WAVEHeader {
    uint32_t riff_id; uint32_t size; uint32_t wav_id; 
};
struct pp_sglp_WavChunk {
    uint32_t id; uint32_t size; 
};
struct pp_sglp_WavFormat {
    int16_t wFormatTag; int16_t nChannels; uint32_t nSamplesPerSec; uint32_t nAvgBytesPerSec; int16_t nBlockAlign; int16_t wBitsPerSample; int16_t cbSize; int16_t wValidBitsPerSample; uint32_t dwChannelMask; uint8_t SubFormat[16]; 
};
struct pp_sglp_RiffIter {
    uint8_t *at; uint8_t *stop; 
};
struct pp_SGLP_XINPUT_GAMEPAD {
    uint16_t wButtons; uint8_t bLeftTrigger; uint8_t bRightTrigger; int16_t sThumbLX; int16_t sThumbLY; int16_t sThumbRX; int16_t sThumbRY; 
};
struct pp_SGLP_XINPUT_STATE {
    uint32_t dwPacketNumber; pp_SGLP_XINPUT_GAMEPAD Gamepad; 
};
struct pp_SGLP_XINPUT_VIBRATION {
    uint16_t wLeftMotorSpeed; uint16_t wRightMotorSpeed; 
};
struct pp_sglp_Win32SoundOutput {
    int32_t samples_per_second; int32_t running_sample_index; int32_t bytes_per_sample; uint32_t secondary_buf_size; uint32_t safety_bytes; 
};
struct pp_WorkQueueEntry {
    pp_void *e; 
};
struct pp_WorkQueue {
    int32_t volatile goal; int32_t volatile cnt; int32_t volatile next_entry_to_write; int32_t volatile next_entry_to_read; pp_void *hsem; pp_WorkQueueEntry entries[256]; 
};
union pp_sglm_V2 {
    float e[2];  struct {float x; float y;  };
};
struct pp_sglm_Mat4x4 {
    pp___m128 e[4]; 
};
struct pp_Entity {
    float x; float y; float scale_x; float scale_y; float rot; 
};
struct pp_Player {
    pp_Entity trans; float start_x; float start_y; pp_Player_Direction dir; float current_frame; 
};
struct pp_Game_State {
    pp_sglp_Sprite player_sprite; pp_sglp_Sprite enemy_one_sprite; pp_sglp_Sprite bitmap_sprite; pp_Player player; pp_Entity enemy[4]; pp_sglp_Sprite text[127]; pp_sglp_Sprite letter; 
};
struct pp_V2 {
    float x; float y; 
};

// Turn a typedef'd type into it's original type.
PP_STATIC pp_Type pp_typedef_to_original(pp_Type type) {
    switch(type) {
        case pp_Type_sglp_Bool: { return(pp_Type_int); } break;
        case pp_Type_sglp_GLenum: { return(pp_Type_uint32_t); } break;
        case pp_Type_sglp_GLbitfield: { return(pp_Type_uint32_t); } break;
        case pp_Type_sglp_GLuint: { return(pp_Type_uint32_t); } break;
        case pp_Type_sglp_GLint: { return(pp_Type_int); } break;
        case pp_Type_sglp_GLsizei: { return(pp_Type_int); } break;
        case pp_Type_sglp_GLboolean: { return(pp_Type_uint8_t); } break;
        case pp_Type_sglp_GLbyte: { return(pp_Type_char); } break;
        case pp_Type_sglp_GLshort: { return(pp_Type_short); } break;
        case pp_Type_sglp_GLubyte: { return(pp_Type_uint8_t); } break;
        case pp_Type_sglp_GLushort: { return(pp_Type_uint16_t); } break;
        case pp_Type_sglp_GLulong: { return(pp_Type_uint32_t); } break;
        case pp_Type_sglp_GLfloat: { return(pp_Type_float); } break;
        case pp_Type_sglp_GLclampf: { return(pp_Type_float); } break;
        case pp_Type_sglp_GLdouble: { return(pp_Type_double); } break;
        case pp_Type_sglp_GLclampd: { return(pp_Type_double); } break;
        case pp_Type_sglp_GLvoid: { return(pp_Type_void); } break;
        case pp_Type_sglp_GLchar: { return(pp_Type_char); } break;
        case pp_Type_sglp_GLsizeiptr: { return(pp_Type_long); } break;
        case pp_Type_sglp_glBindTexture_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glClear_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glClearColor_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glDrawArrays_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGetError_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glTexImage2D_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glTexParameteri_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGetString_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glViewport_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glCreateShader_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glShaderSource_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glCompileShader_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGetShaderiv_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glCreateProgram_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glAttachShader_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glBindAttribLocation_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glLinkProgram_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glUseProgram_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGenBuffers_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glBindBuffer_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glBufferData_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glVertexAttribPointer_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glEnableVertexAttribArray_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glUniform1i_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glUniform2f_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glUniform4f_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glUniformMatrix4fv_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGetUniformLocation_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glIsShader_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGetProgramiv_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGetProgramInfoLog_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGetShaderInfoLog_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glGenVertexArrays_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glBindVertexArray_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_TranslateMessage_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_DispatchMessageA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_PeekMessageA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_DefWindowProcA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_RegisterClassA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_CreateWindowExA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_SetWindowPos_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_GetWindowPlacement_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_SetWindowPlacement_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_GetDC_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_GetCursorPos_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_ScreenToClient_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_GetWindowLongA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_SetWindowLongA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_MonitorFromWindow_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_GetMonitorInfoA_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_GetSystemMetrics_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_ChoosePixelFormat_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_DescribePixelFormat_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_SetPixelFormat_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_SwapBuffers_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_DirectSoundCreate: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XInputGetState_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XInputSetState_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_wglGetProcAddress_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_wglCreateContext_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_wglMakeCurrent_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_wglDeleteContext_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_wglCreateContextAttribsArb_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_wglSwapIntervalExt_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_TimeBeginPeriod_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XOpenDisplay_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XCreateColorMap_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XCreateWindow_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XMapWindow_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XStoreName_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XSelectInput_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XPending_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XNextEvent_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XKeycodeToKeysym_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XGetWindowAttributes_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_XQueryPointer_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_sem_open_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_sem_close_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_pthread_create_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glXGetProcAddressArb_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glXChooseVisual_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glXCreateContext_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glXMakeCurrent_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglp_glXSwapBuffers_t: { return(pp_Type_uintptr_t); } break;
        case pp_Type_sglm_Bool: { return(pp_Type_int); } break;
    }

    return(type);
}
typedef struct pp_MemberDefinition {
    pp_Type type;
    char const *name;
    uintptr_t offset;
    uintptr_t ptr;
    uintptr_t arr_size;
} pp_MemberDefinition;

PP_STATIC pp_MemberDefinition pp_get_members_from_type(pp_Type type, uintptr_t index) {
    pp_Type real_type = pp_typedef_to_original(type);
    if(real_type == pp_Type___m128) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_float, "e", PP_OFFSETOF(pp___m128, e), 0, 4};
                return(res);
            }
        }
    }
    else if(real_type == pp_Type___m128i) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_float, "e", PP_OFFSETOF(pp___m128i, e), 0, 4};
                return(res);
            }
        }
    }
    else if(real_type == pp_Type_sglp_Sprite) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_int32_t, "w", PP_OFFSETOF(pp_sglp_Sprite, w), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_int32_t, "h", PP_OFFSETOF(pp_sglp_Sprite, h), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_int32_t, "id", PP_OFFSETOF(pp_sglp_Sprite, id), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_int32_t, "frame_cnt_x", PP_OFFSETOF(pp_sglp_Sprite, frame_cnt_x), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_int32_t, "frame_cnt_y", PP_OFFSETOF(pp_sglp_Sprite, frame_cnt_y), 0, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "vbo_arr", PP_OFFSETOF(pp_sglp_Sprite, vbo_arr), 0, 2};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "mesh", PP_OFFSETOF(pp_sglp_Sprite, mesh), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_PlayingSound) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_sglp_PlayingSound, "next", PP_OFFSETOF(pp_sglp_PlayingSound, next), 1, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_float, "cur_volume0", PP_OFFSETOF(pp_sglp_PlayingSound, cur_volume0), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_float, "cur_volume1", PP_OFFSETOF(pp_sglp_PlayingSound, cur_volume1), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_float, "target_volume0", PP_OFFSETOF(pp_sglp_PlayingSound, target_volume0), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_float, "target_volume1", PP_OFFSETOF(pp_sglp_PlayingSound, target_volume1), 0, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_float, "dcur_volume0", PP_OFFSETOF(pp_sglp_PlayingSound, dcur_volume0), 0, 0};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_float, "dcur_volume1", PP_OFFSETOF(pp_sglp_PlayingSound, dcur_volume1), 0, 0};
                return(res);
            } break; 
            case 7: {
                pp_MemberDefinition res = {pp_Type_float, "dsample", PP_OFFSETOF(pp_sglp_PlayingSound, dsample), 0, 0};
                return(res);
            } break; 
            case 8: {
                pp_MemberDefinition res = {pp_Type_int32_t, "id", PP_OFFSETOF(pp_sglp_PlayingSound, id), 0, 0};
                return(res);
            } break; 
            case 9: {
                pp_MemberDefinition res = {pp_Type_float, "samples_played", PP_OFFSETOF(pp_sglp_PlayingSound, samples_played), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_OpenGlFunctions) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_sglp_glBindTexture_t, "glBindTexture", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glBindTexture), 1, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_sglp_glClear_t, "glClear", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glClear), 1, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_sglp_glClearColor_t, "glClearColor", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glClearColor), 1, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_sglp_glDrawArrays_t, "glDrawArrays", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glDrawArrays), 1, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_sglp_glGetError_t, "glGetError", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGetError), 1, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_sglp_glTexImage2D_t, "glTexImage2D", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glTexImage2D), 1, 0};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_sglp_glTexParameteri_t, "glTexParameteri", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glTexParameteri), 1, 0};
                return(res);
            } break; 
            case 7: {
                pp_MemberDefinition res = {pp_Type_sglp_glGetString_t, "glGetString", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGetString), 1, 0};
                return(res);
            } break; 
            case 8: {
                pp_MemberDefinition res = {pp_Type_sglp_glViewport_t, "glViewport", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glViewport), 1, 0};
                return(res);
            } break; 
            case 9: {
                pp_MemberDefinition res = {pp_Type_sglp_glCreateShader_t, "glCreateShader", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glCreateShader), 1, 0};
                return(res);
            } break; 
            case 10: {
                pp_MemberDefinition res = {pp_Type_sglp_glShaderSource_t, "glShaderSource", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glShaderSource), 1, 0};
                return(res);
            } break; 
            case 11: {
                pp_MemberDefinition res = {pp_Type_sglp_glCompileShader_t, "glCompileShader", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glCompileShader), 1, 0};
                return(res);
            } break; 
            case 12: {
                pp_MemberDefinition res = {pp_Type_sglp_glGetShaderiv_t, "glGetShaderiv", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGetShaderiv), 1, 0};
                return(res);
            } break; 
            case 13: {
                pp_MemberDefinition res = {pp_Type_sglp_glCreateProgram_t, "glCreateProgram", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glCreateProgram), 1, 0};
                return(res);
            } break; 
            case 14: {
                pp_MemberDefinition res = {pp_Type_sglp_glAttachShader_t, "glAttachShader", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glAttachShader), 1, 0};
                return(res);
            } break; 
            case 15: {
                pp_MemberDefinition res = {pp_Type_sglp_glBindAttribLocation_t, "glBindAttribLocation", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glBindAttribLocation), 1, 0};
                return(res);
            } break; 
            case 16: {
                pp_MemberDefinition res = {pp_Type_sglp_glLinkProgram_t, "glLinkProgram", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glLinkProgram), 1, 0};
                return(res);
            } break; 
            case 17: {
                pp_MemberDefinition res = {pp_Type_sglp_glUseProgram_t, "glUseProgram", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glUseProgram), 1, 0};
                return(res);
            } break; 
            case 18: {
                pp_MemberDefinition res = {pp_Type_sglp_glGenBuffers_t, "glGenBuffers", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGenBuffers), 1, 0};
                return(res);
            } break; 
            case 19: {
                pp_MemberDefinition res = {pp_Type_sglp_glBindBuffer_t, "glBindBuffer", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glBindBuffer), 1, 0};
                return(res);
            } break; 
            case 20: {
                pp_MemberDefinition res = {pp_Type_sglp_glBufferData_t, "glBufferData", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glBufferData), 1, 0};
                return(res);
            } break; 
            case 21: {
                pp_MemberDefinition res = {pp_Type_sglp_glVertexAttribPointer_t, "glVertexAttribPointer", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glVertexAttribPointer), 1, 0};
                return(res);
            } break; 
            case 22: {
                pp_MemberDefinition res = {pp_Type_sglp_glEnableVertexAttribArray_t, "glEnableVertexAttribArray", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glEnableVertexAttribArray), 1, 0};
                return(res);
            } break; 
            case 23: {
                pp_MemberDefinition res = {pp_Type_sglp_glUniform1i_t, "glUniform1i", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glUniform1i), 1, 0};
                return(res);
            } break; 
            case 24: {
                pp_MemberDefinition res = {pp_Type_sglp_glUniform2f_t, "glUniform2f", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glUniform2f), 1, 0};
                return(res);
            } break; 
            case 25: {
                pp_MemberDefinition res = {pp_Type_sglp_glUniform4f_t, "glUniform4f", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glUniform4f), 1, 0};
                return(res);
            } break; 
            case 26: {
                pp_MemberDefinition res = {pp_Type_sglp_glUniformMatrix4fv_t, "glUniformMatrix4fv", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glUniformMatrix4fv), 1, 0};
                return(res);
            } break; 
            case 27: {
                pp_MemberDefinition res = {pp_Type_sglp_glGetUniformLocation_t, "glGetUniformLocation", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGetUniformLocation), 1, 0};
                return(res);
            } break; 
            case 28: {
                pp_MemberDefinition res = {pp_Type_sglp_glIsShader_t, "glIsShader", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glIsShader), 1, 0};
                return(res);
            } break; 
            case 29: {
                pp_MemberDefinition res = {pp_Type_sglp_glGetProgramiv_t, "glGetProgramiv", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGetProgramiv), 1, 0};
                return(res);
            } break; 
            case 30: {
                pp_MemberDefinition res = {pp_Type_sglp_glGetProgramInfoLog_t, "glGetProgramInfoLog", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGetProgramInfoLog), 1, 0};
                return(res);
            } break; 
            case 31: {
                pp_MemberDefinition res = {pp_Type_sglp_glGetShaderInfoLog_t, "glGetShaderInfoLog", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGetShaderInfoLog), 1, 0};
                return(res);
            } break; 
            case 32: {
                pp_MemberDefinition res = {pp_Type_sglp_glGenVertexArrays_t, "glGenVertexArrays", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glGenVertexArrays), 1, 0};
                return(res);
            } break; 
            case 33: {
                pp_MemberDefinition res = {pp_Type_sglp_glBindVertexArray_t, "glBindVertexArray", PP_OFFSETOF(pp_sglp_OpenGlFunctions, glBindVertexArray), 1, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_Settings) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_sglp_Bool, "fullscreen", PP_OFFSETOF(pp_sglp_Settings, fullscreen), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_int32_t, "win_width", PP_OFFSETOF(pp_sglp_Settings, win_width), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_int32_t, "win_height", PP_OFFSETOF(pp_sglp_Settings, win_height), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_int32_t, "frame_rate", PP_OFFSETOF(pp_sglp_Settings, frame_rate), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_uintptr_t, "permanent_memory_size", PP_OFFSETOF(pp_sglp_Settings, permanent_memory_size), 0, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_int32_t, "max_no_of_sounds", PP_OFFSETOF(pp_sglp_Settings, max_no_of_sounds), 0, 0};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_char, "window_title", PP_OFFSETOF(pp_sglp_Settings, window_title), 1, 0};
                return(res);
            } break; 
            case 7: {
                pp_MemberDefinition res = {pp_Type_int32_t, "thread_cnt", PP_OFFSETOF(pp_sglp_Settings, thread_cnt), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_File) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint8_t, "e", PP_OFFSETOF(pp_sglp_File, e), 1, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_uintptr_t, "size", PP_OFFSETOF(pp_sglp_File, size), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_API) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_sglp_Settings, "settings", PP_OFFSETOF(pp_sglp_API, settings), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_sglp_OpenGlFunctions, "gl", PP_OFFSETOF(pp_sglp_API, gl), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_void, "permanent_memory", PP_OFFSETOF(pp_sglp_API, permanent_memory), 1, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_float, "key", PP_OFFSETOF(pp_sglp_API, key), 0, 256};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_float, "dt", PP_OFFSETOF(pp_sglp_API, dt), 0, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_sglp_Bool, "init_game", PP_OFFSETOF(pp_sglp_API, init_game), 0, 0};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_sglp_Bool, "quit", PP_OFFSETOF(pp_sglp_API, quit), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_LoadedSound) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "sample_cnt", PP_OFFSETOF(pp_sglp_LoadedSound, sample_cnt), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "no_channels", PP_OFFSETOF(pp_sglp_LoadedSound, no_channels), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_int16_t, "samples", PP_OFFSETOF(pp_sglp_LoadedSound, samples), 1, 2};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_int32_t, "id", PP_OFFSETOF(pp_sglp_LoadedSound, id), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_SoundOutputBuffer) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_int16_t, "samples", PP_OFFSETOF(pp_sglp_SoundOutputBuffer, samples), 1, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_int32_t, "samples_per_second", PP_OFFSETOF(pp_sglp_SoundOutputBuffer, samples_per_second), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_int32_t, "sample_cnt", PP_OFFSETOF(pp_sglp_SoundOutputBuffer, sample_cnt), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_AudioState) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_sglp_PlayingSound, "first_playing_snd", PP_OFFSETOF(pp_sglp_AudioState, first_playing_snd), 1, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_sglp_PlayingSound, "first_free_playing_snd", PP_OFFSETOF(pp_sglp_AudioState, first_free_playing_snd), 1, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_WAVEHeader) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "riff_id", PP_OFFSETOF(pp_sglp_WAVEHeader, riff_id), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "size", PP_OFFSETOF(pp_sglp_WAVEHeader, size), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "wav_id", PP_OFFSETOF(pp_sglp_WAVEHeader, wav_id), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_WavChunk) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "id", PP_OFFSETOF(pp_sglp_WavChunk, id), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "size", PP_OFFSETOF(pp_sglp_WavChunk, size), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_WavFormat) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_int16_t, "wFormatTag", PP_OFFSETOF(pp_sglp_WavFormat, wFormatTag), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_int16_t, "nChannels", PP_OFFSETOF(pp_sglp_WavFormat, nChannels), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "nSamplesPerSec", PP_OFFSETOF(pp_sglp_WavFormat, nSamplesPerSec), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "nAvgBytesPerSec", PP_OFFSETOF(pp_sglp_WavFormat, nAvgBytesPerSec), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_int16_t, "nBlockAlign", PP_OFFSETOF(pp_sglp_WavFormat, nBlockAlign), 0, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_int16_t, "wBitsPerSample", PP_OFFSETOF(pp_sglp_WavFormat, wBitsPerSample), 0, 0};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_int16_t, "cbSize", PP_OFFSETOF(pp_sglp_WavFormat, cbSize), 0, 0};
                return(res);
            } break; 
            case 7: {
                pp_MemberDefinition res = {pp_Type_int16_t, "wValidBitsPerSample", PP_OFFSETOF(pp_sglp_WavFormat, wValidBitsPerSample), 0, 0};
                return(res);
            } break; 
            case 8: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "dwChannelMask", PP_OFFSETOF(pp_sglp_WavFormat, dwChannelMask), 0, 0};
                return(res);
            } break; 
            case 9: {
                pp_MemberDefinition res = {pp_Type_uint8_t, "SubFormat", PP_OFFSETOF(pp_sglp_WavFormat, SubFormat), 0, 16};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_RiffIter) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint8_t, "at", PP_OFFSETOF(pp_sglp_RiffIter, at), 1, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_uint8_t, "stop", PP_OFFSETOF(pp_sglp_RiffIter, stop), 1, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_SGLP_XINPUT_GAMEPAD) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint16_t, "wButtons", PP_OFFSETOF(pp_SGLP_XINPUT_GAMEPAD, wButtons), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_uint8_t, "bLeftTrigger", PP_OFFSETOF(pp_SGLP_XINPUT_GAMEPAD, bLeftTrigger), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_uint8_t, "bRightTrigger", PP_OFFSETOF(pp_SGLP_XINPUT_GAMEPAD, bRightTrigger), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_int16_t, "sThumbLX", PP_OFFSETOF(pp_SGLP_XINPUT_GAMEPAD, sThumbLX), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_int16_t, "sThumbLY", PP_OFFSETOF(pp_SGLP_XINPUT_GAMEPAD, sThumbLY), 0, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_int16_t, "sThumbRX", PP_OFFSETOF(pp_SGLP_XINPUT_GAMEPAD, sThumbRX), 0, 0};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_int16_t, "sThumbRY", PP_OFFSETOF(pp_SGLP_XINPUT_GAMEPAD, sThumbRY), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_SGLP_XINPUT_STATE) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "dwPacketNumber", PP_OFFSETOF(pp_SGLP_XINPUT_STATE, dwPacketNumber), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_SGLP_XINPUT_GAMEPAD, "Gamepad", PP_OFFSETOF(pp_SGLP_XINPUT_STATE, Gamepad), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_SGLP_XINPUT_VIBRATION) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_uint16_t, "wLeftMotorSpeed", PP_OFFSETOF(pp_SGLP_XINPUT_VIBRATION, wLeftMotorSpeed), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_uint16_t, "wRightMotorSpeed", PP_OFFSETOF(pp_SGLP_XINPUT_VIBRATION, wRightMotorSpeed), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglp_Win32SoundOutput) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_int32_t, "samples_per_second", PP_OFFSETOF(pp_sglp_Win32SoundOutput, samples_per_second), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_int32_t, "running_sample_index", PP_OFFSETOF(pp_sglp_Win32SoundOutput, running_sample_index), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_int32_t, "bytes_per_sample", PP_OFFSETOF(pp_sglp_Win32SoundOutput, bytes_per_sample), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "secondary_buf_size", PP_OFFSETOF(pp_sglp_Win32SoundOutput, secondary_buf_size), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_uint32_t, "safety_bytes", PP_OFFSETOF(pp_sglp_Win32SoundOutput, safety_bytes), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_WorkQueueEntry) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_void, "e", PP_OFFSETOF(pp_WorkQueueEntry, e), 1, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_WorkQueue) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_int32_t, "goal", PP_OFFSETOF(pp_WorkQueue, goal), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_int32_t, "cnt", PP_OFFSETOF(pp_WorkQueue, cnt), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_int32_t, "next_entry_to_write", PP_OFFSETOF(pp_WorkQueue, next_entry_to_write), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_int32_t, "next_entry_to_read", PP_OFFSETOF(pp_WorkQueue, next_entry_to_read), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_void, "hsem", PP_OFFSETOF(pp_WorkQueue, hsem), 1, 0};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_WorkQueueEntry, "entries", PP_OFFSETOF(pp_WorkQueue, entries), 0, 256};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglm_V2) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_float, "e", PP_OFFSETOF(pp_sglm_V2, e), 0, 2};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_float, "x", PP_OFFSETOF(pp_sglm_V2, x), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_float, "y", PP_OFFSETOF(pp_sglm_V2, y), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglm_Mat4x4) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type___m128, "e", PP_OFFSETOF(pp_sglm_Mat4x4, e), 0, 4};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_Entity) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_float, "x", PP_OFFSETOF(pp_Entity, x), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_float, "y", PP_OFFSETOF(pp_Entity, y), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_float, "scale_x", PP_OFFSETOF(pp_Entity, scale_x), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_float, "scale_y", PP_OFFSETOF(pp_Entity, scale_y), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_float, "rot", PP_OFFSETOF(pp_Entity, rot), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_Player) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_Entity, "trans", PP_OFFSETOF(pp_Player, trans), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_float, "start_x", PP_OFFSETOF(pp_Player, start_x), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_float, "start_y", PP_OFFSETOF(pp_Player, start_y), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_Player_Direction, "dir", PP_OFFSETOF(pp_Player, dir), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_float, "current_frame", PP_OFFSETOF(pp_Player, current_frame), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_Game_State) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_sglp_Sprite, "player_sprite", PP_OFFSETOF(pp_Game_State, player_sprite), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_sglp_Sprite, "enemy_one_sprite", PP_OFFSETOF(pp_Game_State, enemy_one_sprite), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_sglp_Sprite, "bitmap_sprite", PP_OFFSETOF(pp_Game_State, bitmap_sprite), 0, 0};
                return(res);
            } break; 
            case 3: {
                pp_MemberDefinition res = {pp_Type_Player, "player", PP_OFFSETOF(pp_Game_State, player), 0, 0};
                return(res);
            } break; 
            case 4: {
                pp_MemberDefinition res = {pp_Type_Entity, "enemy", PP_OFFSETOF(pp_Game_State, enemy), 0, 4};
                return(res);
            } break; 
            case 5: {
                pp_MemberDefinition res = {pp_Type_sglp_Sprite, "text", PP_OFFSETOF(pp_Game_State, text), 0, 127};
                return(res);
            } break; 
            case 6: {
                pp_MemberDefinition res = {pp_Type_sglp_Sprite, "letter", PP_OFFSETOF(pp_Game_State, letter), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_V2) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_float, "x", PP_OFFSETOF(pp_V2, x), 0, 0};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_float, "y", PP_OFFSETOF(pp_V2, y), 0, 0};
                return(res);
            } break; 
        }
    }

    // Not found
    PP_ASSERT(0);
    pp_MemberDefinition failres; PP_MEMSET(&failres, 0, sizeof(failres)); // Zero all the results for failure case.
    return(failres);
}

PP_STATIC uintptr_t pp_get_number_of_members(pp_Type type) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type___m128: case pp_Type___m128i: { return(1); }
        case pp_Type_sglp_Sprite: { return(7); } break;
        case pp_Type_sglp_PlayingSound: { return(10); } break;
        case pp_Type_sglp_OpenGlFunctions: { return(34); } break;
        case pp_Type_sglp_Settings: { return(8); } break;
        case pp_Type_sglp_File: { return(2); } break;
        case pp_Type_sglp_API: { return(7); } break;
        case pp_Type_sglp_LoadedSound: { return(4); } break;
        case pp_Type_sglp_SoundOutputBuffer: { return(3); } break;
        case pp_Type_sglp_AudioState: { return(2); } break;
        case pp_Type_sglp_WAVEHeader: { return(3); } break;
        case pp_Type_sglp_WavChunk: { return(2); } break;
        case pp_Type_sglp_WavFormat: { return(10); } break;
        case pp_Type_sglp_RiffIter: { return(2); } break;
        case pp_Type_SGLP_XINPUT_GAMEPAD: { return(7); } break;
        case pp_Type_SGLP_XINPUT_STATE: { return(2); } break;
        case pp_Type_SGLP_XINPUT_VIBRATION: { return(2); } break;
        case pp_Type_sglp_Win32SoundOutput: { return(5); } break;
        case pp_Type_WorkQueueEntry: { return(1); } break;
        case pp_Type_WorkQueue: { return(6); } break;
        case pp_Type_sglm_V2: { return(3); } break;
        case pp_Type_sglm_Mat4x4: { return(1); } break;
        case pp_Type_Entity: { return(5); } break;
        case pp_Type_Player: { return(5); } break;
        case pp_Type_Game_State: { return(7); } break;
        case pp_Type_V2: { return(2); } break;
    }

    PP_ASSERT(0);
    return(0);
}

typedef enum pp_StructureType {
    pp_StructureType_unknown,

    pp_StructureType_primitive,
    pp_StructureType_struct,
    pp_StructureType_enum,

    pp_StructureType_count
} pp_StructureType;

PP_STATIC pp_StructureType pp_get_structure_type(pp_Type type) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type_char: case pp_Type_short: case pp_Type_int: case pp_Type_long: case pp_Type_float: case pp_Type_double: case pp_Type_uint64_t: case pp_Type_uint32_t: case pp_Type_uint16_t: case pp_Type_uint8_t: case pp_Type_int64_t: case pp_Type_int32_t: case pp_Type_int16_t: case pp_Type_int8_t: case pp_Type_uintptr_t: case pp_Type_intptr_t: case pp_Type_size_t: {
            return(pp_StructureType_primitive);
        } break;

        case pp_Type_sglp_Key: case pp_Type_Player_Direction: case pp_Type_ID: {
            return(pp_StructureType_enum);
        } break;

        case pp_Type___m128: case pp_Type___m128i: case pp_Type_sglp_Sprite: case pp_Type_sglp_PlayingSound: case pp_Type_sglp_OpenGlFunctions: case pp_Type_sglp_Settings: case pp_Type_sglp_File: case pp_Type_sglp_API: case pp_Type_sglp_LoadedSound: case pp_Type_sglp_SoundOutputBuffer: case pp_Type_sglp_AudioState: case pp_Type_sglp_WAVEHeader: case pp_Type_sglp_WavChunk: case pp_Type_sglp_WavFormat: case pp_Type_sglp_RiffIter: case pp_Type_SGLP_XINPUT_GAMEPAD: case pp_Type_SGLP_XINPUT_STATE: case pp_Type_SGLP_XINPUT_VIBRATION: case pp_Type_sglp_Win32SoundOutput: case pp_Type_WorkQueueEntry: case pp_Type_WorkQueue: case pp_Type_sglm_V2: case pp_Type_sglm_Mat4x4: case pp_Type_Entity: case pp_Type_Player: case pp_Type_Game_State: case pp_Type_V2: {
            return(pp_StructureType_struct);
        } break;
    }

    PP_ASSERT(0);
    return(pp_StructureType_unknown);
}

PP_STATIC char const * pp_type_to_string(pp_Type type) {
    switch(type) {
        case pp_Type_char: { return("char"); } break;
        case pp_Type_short: { return("short"); } break;
        case pp_Type_int: { return("int"); } break;
        case pp_Type_long: { return("long"); } break;
        case pp_Type_float: { return("float"); } break;
        case pp_Type_double: { return("double"); } break;
        case pp_Type_uint64_t: { return("uint64_t"); } break;
        case pp_Type_uint32_t: { return("uint32_t"); } break;
        case pp_Type_uint16_t: { return("uint16_t"); } break;
        case pp_Type_uint8_t: { return("uint8_t"); } break;
        case pp_Type_int64_t: { return("int64_t"); } break;
        case pp_Type_int32_t: { return("int32_t"); } break;
        case pp_Type_int16_t: { return("int16_t"); } break;
        case pp_Type_int8_t: { return("int8_t"); } break;
        case pp_Type_uintptr_t: { return("uintptr_t"); } break;
        case pp_Type_intptr_t: { return("intptr_t"); } break;
        case pp_Type_size_t: { return("size_t"); } break;
        case pp_Type_sglp_Bool: { return("sglp_Bool"); } break;
        case pp_Type_sglp_GLenum: { return("sglp_GLenum"); } break;
        case pp_Type_sglp_GLbitfield: { return("sglp_GLbitfield"); } break;
        case pp_Type_sglp_GLuint: { return("sglp_GLuint"); } break;
        case pp_Type_sglp_GLint: { return("sglp_GLint"); } break;
        case pp_Type_sglp_GLsizei: { return("sglp_GLsizei"); } break;
        case pp_Type_sglp_GLboolean: { return("sglp_GLboolean"); } break;
        case pp_Type_sglp_GLbyte: { return("sglp_GLbyte"); } break;
        case pp_Type_sglp_GLshort: { return("sglp_GLshort"); } break;
        case pp_Type_sglp_GLubyte: { return("sglp_GLubyte"); } break;
        case pp_Type_sglp_GLushort: { return("sglp_GLushort"); } break;
        case pp_Type_sglp_GLulong: { return("sglp_GLulong"); } break;
        case pp_Type_sglp_GLfloat: { return("sglp_GLfloat"); } break;
        case pp_Type_sglp_GLclampf: { return("sglp_GLclampf"); } break;
        case pp_Type_sglp_GLdouble: { return("sglp_GLdouble"); } break;
        case pp_Type_sglp_GLclampd: { return("sglp_GLclampd"); } break;
        case pp_Type_sglp_GLvoid: { return("sglp_GLvoid"); } break;
        case pp_Type_sglp_GLchar: { return("sglp_GLchar"); } break;
        case pp_Type_sglp_GLsizeiptr: { return("sglp_GLsizeiptr"); } break;
        case pp_Type_sglp_glBindTexture_t: { return("sglp_glBindTexture_t"); } break;
        case pp_Type_sglp_glClear_t: { return("sglp_glClear_t"); } break;
        case pp_Type_sglp_glClearColor_t: { return("sglp_glClearColor_t"); } break;
        case pp_Type_sglp_glDrawArrays_t: { return("sglp_glDrawArrays_t"); } break;
        case pp_Type_sglp_glGetError_t: { return("sglp_glGetError_t"); } break;
        case pp_Type_sglp_glTexImage2D_t: { return("sglp_glTexImage2D_t"); } break;
        case pp_Type_sglp_glTexParameteri_t: { return("sglp_glTexParameteri_t"); } break;
        case pp_Type_sglp_glGetString_t: { return("sglp_glGetString_t"); } break;
        case pp_Type_sglp_glViewport_t: { return("sglp_glViewport_t"); } break;
        case pp_Type_sglp_glCreateShader_t: { return("sglp_glCreateShader_t"); } break;
        case pp_Type_sglp_glShaderSource_t: { return("sglp_glShaderSource_t"); } break;
        case pp_Type_sglp_glCompileShader_t: { return("sglp_glCompileShader_t"); } break;
        case pp_Type_sglp_glGetShaderiv_t: { return("sglp_glGetShaderiv_t"); } break;
        case pp_Type_sglp_glCreateProgram_t: { return("sglp_glCreateProgram_t"); } break;
        case pp_Type_sglp_glAttachShader_t: { return("sglp_glAttachShader_t"); } break;
        case pp_Type_sglp_glBindAttribLocation_t: { return("sglp_glBindAttribLocation_t"); } break;
        case pp_Type_sglp_glLinkProgram_t: { return("sglp_glLinkProgram_t"); } break;
        case pp_Type_sglp_glUseProgram_t: { return("sglp_glUseProgram_t"); } break;
        case pp_Type_sglp_glGenBuffers_t: { return("sglp_glGenBuffers_t"); } break;
        case pp_Type_sglp_glBindBuffer_t: { return("sglp_glBindBuffer_t"); } break;
        case pp_Type_sglp_glBufferData_t: { return("sglp_glBufferData_t"); } break;
        case pp_Type_sglp_glVertexAttribPointer_t: { return("sglp_glVertexAttribPointer_t"); } break;
        case pp_Type_sglp_glEnableVertexAttribArray_t: { return("sglp_glEnableVertexAttribArray_t"); } break;
        case pp_Type_sglp_glUniform1i_t: { return("sglp_glUniform1i_t"); } break;
        case pp_Type_sglp_glUniform2f_t: { return("sglp_glUniform2f_t"); } break;
        case pp_Type_sglp_glUniform4f_t: { return("sglp_glUniform4f_t"); } break;
        case pp_Type_sglp_glUniformMatrix4fv_t: { return("sglp_glUniformMatrix4fv_t"); } break;
        case pp_Type_sglp_glGetUniformLocation_t: { return("sglp_glGetUniformLocation_t"); } break;
        case pp_Type_sglp_glIsShader_t: { return("sglp_glIsShader_t"); } break;
        case pp_Type_sglp_glGetProgramiv_t: { return("sglp_glGetProgramiv_t"); } break;
        case pp_Type_sglp_glGetProgramInfoLog_t: { return("sglp_glGetProgramInfoLog_t"); } break;
        case pp_Type_sglp_glGetShaderInfoLog_t: { return("sglp_glGetShaderInfoLog_t"); } break;
        case pp_Type_sglp_glGenVertexArrays_t: { return("sglp_glGenVertexArrays_t"); } break;
        case pp_Type_sglp_glBindVertexArray_t: { return("sglp_glBindVertexArray_t"); } break;
        case pp_Type_sglp_TranslateMessage_t: { return("sglp_TranslateMessage_t"); } break;
        case pp_Type_sglp_DispatchMessageA_t: { return("sglp_DispatchMessageA_t"); } break;
        case pp_Type_sglp_PeekMessageA_t: { return("sglp_PeekMessageA_t"); } break;
        case pp_Type_sglp_DefWindowProcA_t: { return("sglp_DefWindowProcA_t"); } break;
        case pp_Type_sglp_RegisterClassA_t: { return("sglp_RegisterClassA_t"); } break;
        case pp_Type_sglp_CreateWindowExA_t: { return("sglp_CreateWindowExA_t"); } break;
        case pp_Type_sglp_SetWindowPos_t: { return("sglp_SetWindowPos_t"); } break;
        case pp_Type_sglp_GetWindowPlacement_t: { return("sglp_GetWindowPlacement_t"); } break;
        case pp_Type_sglp_SetWindowPlacement_t: { return("sglp_SetWindowPlacement_t"); } break;
        case pp_Type_sglp_GetDC_t: { return("sglp_GetDC_t"); } break;
        case pp_Type_sglp_GetCursorPos_t: { return("sglp_GetCursorPos_t"); } break;
        case pp_Type_sglp_ScreenToClient_t: { return("sglp_ScreenToClient_t"); } break;
        case pp_Type_sglp_GetWindowLongA_t: { return("sglp_GetWindowLongA_t"); } break;
        case pp_Type_sglp_SetWindowLongA_t: { return("sglp_SetWindowLongA_t"); } break;
        case pp_Type_sglp_MonitorFromWindow_t: { return("sglp_MonitorFromWindow_t"); } break;
        case pp_Type_sglp_GetMonitorInfoA_t: { return("sglp_GetMonitorInfoA_t"); } break;
        case pp_Type_sglp_GetSystemMetrics_t: { return("sglp_GetSystemMetrics_t"); } break;
        case pp_Type_sglp_ChoosePixelFormat_t: { return("sglp_ChoosePixelFormat_t"); } break;
        case pp_Type_sglp_DescribePixelFormat_t: { return("sglp_DescribePixelFormat_t"); } break;
        case pp_Type_sglp_SetPixelFormat_t: { return("sglp_SetPixelFormat_t"); } break;
        case pp_Type_sglp_SwapBuffers_t: { return("sglp_SwapBuffers_t"); } break;
        case pp_Type_DirectSoundCreate: { return("DirectSoundCreate"); } break;
        case pp_Type_sglp_XInputGetState_t: { return("sglp_XInputGetState_t"); } break;
        case pp_Type_sglp_XInputSetState_t: { return("sglp_XInputSetState_t"); } break;
        case pp_Type_wglGetProcAddress_t: { return("wglGetProcAddress_t"); } break;
        case pp_Type_wglCreateContext_t: { return("wglCreateContext_t"); } break;
        case pp_Type_wglMakeCurrent_t: { return("wglMakeCurrent_t"); } break;
        case pp_Type_wglDeleteContext_t: { return("wglDeleteContext_t"); } break;
        case pp_Type_wglCreateContextAttribsArb_t: { return("wglCreateContextAttribsArb_t"); } break;
        case pp_Type_wglSwapIntervalExt_t: { return("wglSwapIntervalExt_t"); } break;
        case pp_Type_TimeBeginPeriod_t: { return("TimeBeginPeriod_t"); } break;
        case pp_Type_sglp_XOpenDisplay_t: { return("sglp_XOpenDisplay_t"); } break;
        case pp_Type_sglp_XCreateColorMap_t: { return("sglp_XCreateColorMap_t"); } break;
        case pp_Type_sglp_XCreateWindow_t: { return("sglp_XCreateWindow_t"); } break;
        case pp_Type_sglp_XMapWindow_t: { return("sglp_XMapWindow_t"); } break;
        case pp_Type_sglp_XStoreName_t: { return("sglp_XStoreName_t"); } break;
        case pp_Type_sglp_XSelectInput_t: { return("sglp_XSelectInput_t"); } break;
        case pp_Type_sglp_XPending_t: { return("sglp_XPending_t"); } break;
        case pp_Type_sglp_XNextEvent_t: { return("sglp_XNextEvent_t"); } break;
        case pp_Type_sglp_XKeycodeToKeysym_t: { return("sglp_XKeycodeToKeysym_t"); } break;
        case pp_Type_sglp_XGetWindowAttributes_t: { return("sglp_XGetWindowAttributes_t"); } break;
        case pp_Type_sglp_XQueryPointer_t: { return("sglp_XQueryPointer_t"); } break;
        case pp_Type_sglp_sem_open_t: { return("sglp_sem_open_t"); } break;
        case pp_Type_sglp_sem_close_t: { return("sglp_sem_close_t"); } break;
        case pp_Type_sglp_pthread_create_t: { return("sglp_pthread_create_t"); } break;
        case pp_Type_sglp_glXGetProcAddressArb_t: { return("sglp_glXGetProcAddressArb_t"); } break;
        case pp_Type_sglp_glXChooseVisual_t: { return("sglp_glXChooseVisual_t"); } break;
        case pp_Type_sglp_glXCreateContext_t: { return("sglp_glXCreateContext_t"); } break;
        case pp_Type_sglp_glXMakeCurrent_t: { return("sglp_glXMakeCurrent_t"); } break;
        case pp_Type_sglp_glXSwapBuffers_t: { return("sglp_glXSwapBuffers_t"); } break;
        case pp_Type_sglm_Bool: { return("sglm_Bool"); } break;
        case pp_Type_sglp_Key: { return("sglp_Key"); } break;
        case pp_Type_Player_Direction: { return("Player_Direction"); } break;
        case pp_Type_ID: { return("ID"); } break;
        case pp_Type_sglp_Sprite: { return("sglp_Sprite"); } break;
        case pp_Type_sglp_PlayingSound: { return("sglp_PlayingSound"); } break;
        case pp_Type_sglp_OpenGlFunctions: { return("sglp_OpenGlFunctions"); } break;
        case pp_Type_sglp_Settings: { return("sglp_Settings"); } break;
        case pp_Type_sglp_File: { return("sglp_File"); } break;
        case pp_Type_sglp_API: { return("sglp_API"); } break;
        case pp_Type_void: { return("void"); } break;
        case pp_Type_sglp_LoadedSound: { return("sglp_LoadedSound"); } break;
        case pp_Type_sglp_SoundOutputBuffer: { return("sglp_SoundOutputBuffer"); } break;
        case pp_Type_sglp_AudioState: { return("sglp_AudioState"); } break;
        case pp_Type_sglp_WAVEHeader: { return("sglp_WAVEHeader"); } break;
        case pp_Type_sglp_WavChunk: { return("sglp_WavChunk"); } break;
        case pp_Type_sglp_WavFormat: { return("sglp_WavFormat"); } break;
        case pp_Type_sglp_RiffIter: { return("sglp_RiffIter"); } break;
        case pp_Type_SGLP_XINPUT_GAMEPAD: { return("SGLP_XINPUT_GAMEPAD"); } break;
        case pp_Type_SGLP_XINPUT_STATE: { return("SGLP_XINPUT_STATE"); } break;
        case pp_Type_SGLP_XINPUT_VIBRATION: { return("SGLP_XINPUT_VIBRATION"); } break;
        case pp_Type_sglp_Win32SoundOutput: { return("sglp_Win32SoundOutput"); } break;
        case pp_Type_WorkQueueEntry: { return("WorkQueueEntry"); } break;
        case pp_Type_WorkQueue: { return("WorkQueue"); } break;
        case pp_Type_sglm_V2: { return("sglm_V2"); } break;
        case pp_Type_sglm_Mat4x4: { return("sglm_Mat4x4"); } break;
        case pp_Type___m128: { return("__m128"); } break;
        case pp_Type_Entity: { return("Entity"); } break;
        case pp_Type_Player: { return("Player"); } break;
        case pp_Type_Game_State: { return("Game_State"); } break;
        case pp_Type_V2: { return("V2"); } break;
        case pp_Type___m128i: { return("__m128i"); } break;
    }
    
    PP_ASSERT(0);
    return(0);
}

PP_STATIC uintptr_t pp_get_size_from_type(pp_Type type) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type_char:
            return sizeof(pp_char);
            break;
        case pp_Type_short:
            return sizeof(pp_short);
            break;
        case pp_Type_int:
            return sizeof(pp_int);
            break;
        case pp_Type_long:
            return sizeof(pp_long);
            break;
        case pp_Type_float:
            return sizeof(pp_float);
            break;
        case pp_Type_double:
            return sizeof(pp_double);
            break;
        case pp_Type_uint64_t:
            return sizeof(pp_uint64_t);
            break;
        case pp_Type_uint32_t:
            return sizeof(pp_uint32_t);
            break;
        case pp_Type_uint16_t:
            return sizeof(pp_uint16_t);
            break;
        case pp_Type_uint8_t:
            return sizeof(pp_uint8_t);
            break;
        case pp_Type_int64_t:
            return sizeof(pp_int64_t);
            break;
        case pp_Type_int32_t:
            return sizeof(pp_int32_t);
            break;
        case pp_Type_int16_t:
            return sizeof(pp_int16_t);
            break;
        case pp_Type_int8_t:
            return sizeof(pp_int8_t);
            break;
        case pp_Type_uintptr_t:
            return sizeof(pp_uintptr_t);
            break;
        case pp_Type_intptr_t:
            return sizeof(pp_intptr_t);
            break;
        case pp_Type_size_t:
            return sizeof(pp_size_t);
            break;
        case pp_Type_sglp_Bool:
            return sizeof(pp_sglp_Bool);
            break;
        case pp_Type_sglp_GLenum:
            return sizeof(pp_sglp_GLenum);
            break;
        case pp_Type_sglp_GLbitfield:
            return sizeof(pp_sglp_GLbitfield);
            break;
        case pp_Type_sglp_GLuint:
            return sizeof(pp_sglp_GLuint);
            break;
        case pp_Type_sglp_GLint:
            return sizeof(pp_sglp_GLint);
            break;
        case pp_Type_sglp_GLsizei:
            return sizeof(pp_sglp_GLsizei);
            break;
        case pp_Type_sglp_GLboolean:
            return sizeof(pp_sglp_GLboolean);
            break;
        case pp_Type_sglp_GLbyte:
            return sizeof(pp_sglp_GLbyte);
            break;
        case pp_Type_sglp_GLshort:
            return sizeof(pp_sglp_GLshort);
            break;
        case pp_Type_sglp_GLubyte:
            return sizeof(pp_sglp_GLubyte);
            break;
        case pp_Type_sglp_GLushort:
            return sizeof(pp_sglp_GLushort);
            break;
        case pp_Type_sglp_GLulong:
            return sizeof(pp_sglp_GLulong);
            break;
        case pp_Type_sglp_GLfloat:
            return sizeof(pp_sglp_GLfloat);
            break;
        case pp_Type_sglp_GLclampf:
            return sizeof(pp_sglp_GLclampf);
            break;
        case pp_Type_sglp_GLdouble:
            return sizeof(pp_sglp_GLdouble);
            break;
        case pp_Type_sglp_GLclampd:
            return sizeof(pp_sglp_GLclampd);
            break;
        case pp_Type_sglp_GLchar:
            return sizeof(pp_sglp_GLchar);
            break;
        case pp_Type_sglp_GLsizeiptr:
            return sizeof(pp_sglp_GLsizeiptr);
            break;
        case pp_Type_sglp_glBindTexture_t:
            return sizeof(pp_sglp_glBindTexture_t);
            break;
        case pp_Type_sglp_glClear_t:
            return sizeof(pp_sglp_glClear_t);
            break;
        case pp_Type_sglp_glClearColor_t:
            return sizeof(pp_sglp_glClearColor_t);
            break;
        case pp_Type_sglp_glDrawArrays_t:
            return sizeof(pp_sglp_glDrawArrays_t);
            break;
        case pp_Type_sglp_glGetError_t:
            return sizeof(pp_sglp_glGetError_t);
            break;
        case pp_Type_sglp_glTexImage2D_t:
            return sizeof(pp_sglp_glTexImage2D_t);
            break;
        case pp_Type_sglp_glTexParameteri_t:
            return sizeof(pp_sglp_glTexParameteri_t);
            break;
        case pp_Type_sglp_glGetString_t:
            return sizeof(pp_sglp_glGetString_t);
            break;
        case pp_Type_sglp_glViewport_t:
            return sizeof(pp_sglp_glViewport_t);
            break;
        case pp_Type_sglp_glCreateShader_t:
            return sizeof(pp_sglp_glCreateShader_t);
            break;
        case pp_Type_sglp_glShaderSource_t:
            return sizeof(pp_sglp_glShaderSource_t);
            break;
        case pp_Type_sglp_glCompileShader_t:
            return sizeof(pp_sglp_glCompileShader_t);
            break;
        case pp_Type_sglp_glGetShaderiv_t:
            return sizeof(pp_sglp_glGetShaderiv_t);
            break;
        case pp_Type_sglp_glCreateProgram_t:
            return sizeof(pp_sglp_glCreateProgram_t);
            break;
        case pp_Type_sglp_glAttachShader_t:
            return sizeof(pp_sglp_glAttachShader_t);
            break;
        case pp_Type_sglp_glBindAttribLocation_t:
            return sizeof(pp_sglp_glBindAttribLocation_t);
            break;
        case pp_Type_sglp_glLinkProgram_t:
            return sizeof(pp_sglp_glLinkProgram_t);
            break;
        case pp_Type_sglp_glUseProgram_t:
            return sizeof(pp_sglp_glUseProgram_t);
            break;
        case pp_Type_sglp_glGenBuffers_t:
            return sizeof(pp_sglp_glGenBuffers_t);
            break;
        case pp_Type_sglp_glBindBuffer_t:
            return sizeof(pp_sglp_glBindBuffer_t);
            break;
        case pp_Type_sglp_glBufferData_t:
            return sizeof(pp_sglp_glBufferData_t);
            break;
        case pp_Type_sglp_glVertexAttribPointer_t:
            return sizeof(pp_sglp_glVertexAttribPointer_t);
            break;
        case pp_Type_sglp_glEnableVertexAttribArray_t:
            return sizeof(pp_sglp_glEnableVertexAttribArray_t);
            break;
        case pp_Type_sglp_glUniform1i_t:
            return sizeof(pp_sglp_glUniform1i_t);
            break;
        case pp_Type_sglp_glUniform2f_t:
            return sizeof(pp_sglp_glUniform2f_t);
            break;
        case pp_Type_sglp_glUniform4f_t:
            return sizeof(pp_sglp_glUniform4f_t);
            break;
        case pp_Type_sglp_glUniformMatrix4fv_t:
            return sizeof(pp_sglp_glUniformMatrix4fv_t);
            break;
        case pp_Type_sglp_glGetUniformLocation_t:
            return sizeof(pp_sglp_glGetUniformLocation_t);
            break;
        case pp_Type_sglp_glIsShader_t:
            return sizeof(pp_sglp_glIsShader_t);
            break;
        case pp_Type_sglp_glGetProgramiv_t:
            return sizeof(pp_sglp_glGetProgramiv_t);
            break;
        case pp_Type_sglp_glGetProgramInfoLog_t:
            return sizeof(pp_sglp_glGetProgramInfoLog_t);
            break;
        case pp_Type_sglp_glGetShaderInfoLog_t:
            return sizeof(pp_sglp_glGetShaderInfoLog_t);
            break;
        case pp_Type_sglp_glGenVertexArrays_t:
            return sizeof(pp_sglp_glGenVertexArrays_t);
            break;
        case pp_Type_sglp_glBindVertexArray_t:
            return sizeof(pp_sglp_glBindVertexArray_t);
            break;
        case pp_Type_sglp_TranslateMessage_t:
            return sizeof(pp_sglp_TranslateMessage_t);
            break;
        case pp_Type_sglp_DispatchMessageA_t:
            return sizeof(pp_sglp_DispatchMessageA_t);
            break;
        case pp_Type_sglp_PeekMessageA_t:
            return sizeof(pp_sglp_PeekMessageA_t);
            break;
        case pp_Type_sglp_DefWindowProcA_t:
            return sizeof(pp_sglp_DefWindowProcA_t);
            break;
        case pp_Type_sglp_RegisterClassA_t:
            return sizeof(pp_sglp_RegisterClassA_t);
            break;
        case pp_Type_sglp_CreateWindowExA_t:
            return sizeof(pp_sglp_CreateWindowExA_t);
            break;
        case pp_Type_sglp_SetWindowPos_t:
            return sizeof(pp_sglp_SetWindowPos_t);
            break;
        case pp_Type_sglp_GetWindowPlacement_t:
            return sizeof(pp_sglp_GetWindowPlacement_t);
            break;
        case pp_Type_sglp_SetWindowPlacement_t:
            return sizeof(pp_sglp_SetWindowPlacement_t);
            break;
        case pp_Type_sglp_GetDC_t:
            return sizeof(pp_sglp_GetDC_t);
            break;
        case pp_Type_sglp_GetCursorPos_t:
            return sizeof(pp_sglp_GetCursorPos_t);
            break;
        case pp_Type_sglp_ScreenToClient_t:
            return sizeof(pp_sglp_ScreenToClient_t);
            break;
        case pp_Type_sglp_GetWindowLongA_t:
            return sizeof(pp_sglp_GetWindowLongA_t);
            break;
        case pp_Type_sglp_SetWindowLongA_t:
            return sizeof(pp_sglp_SetWindowLongA_t);
            break;
        case pp_Type_sglp_MonitorFromWindow_t:
            return sizeof(pp_sglp_MonitorFromWindow_t);
            break;
        case pp_Type_sglp_GetMonitorInfoA_t:
            return sizeof(pp_sglp_GetMonitorInfoA_t);
            break;
        case pp_Type_sglp_GetSystemMetrics_t:
            return sizeof(pp_sglp_GetSystemMetrics_t);
            break;
        case pp_Type_sglp_ChoosePixelFormat_t:
            return sizeof(pp_sglp_ChoosePixelFormat_t);
            break;
        case pp_Type_sglp_DescribePixelFormat_t:
            return sizeof(pp_sglp_DescribePixelFormat_t);
            break;
        case pp_Type_sglp_SetPixelFormat_t:
            return sizeof(pp_sglp_SetPixelFormat_t);
            break;
        case pp_Type_sglp_SwapBuffers_t:
            return sizeof(pp_sglp_SwapBuffers_t);
            break;
        case pp_Type_DirectSoundCreate:
            return sizeof(pp_DirectSoundCreate);
            break;
        case pp_Type_sglp_XInputGetState_t:
            return sizeof(pp_sglp_XInputGetState_t);
            break;
        case pp_Type_sglp_XInputSetState_t:
            return sizeof(pp_sglp_XInputSetState_t);
            break;
        case pp_Type_wglGetProcAddress_t:
            return sizeof(pp_wglGetProcAddress_t);
            break;
        case pp_Type_wglCreateContext_t:
            return sizeof(pp_wglCreateContext_t);
            break;
        case pp_Type_wglMakeCurrent_t:
            return sizeof(pp_wglMakeCurrent_t);
            break;
        case pp_Type_wglDeleteContext_t:
            return sizeof(pp_wglDeleteContext_t);
            break;
        case pp_Type_wglCreateContextAttribsArb_t:
            return sizeof(pp_wglCreateContextAttribsArb_t);
            break;
        case pp_Type_wglSwapIntervalExt_t:
            return sizeof(pp_wglSwapIntervalExt_t);
            break;
        case pp_Type_TimeBeginPeriod_t:
            return sizeof(pp_TimeBeginPeriod_t);
            break;
        case pp_Type_sglp_XOpenDisplay_t:
            return sizeof(pp_sglp_XOpenDisplay_t);
            break;
        case pp_Type_sglp_XCreateColorMap_t:
            return sizeof(pp_sglp_XCreateColorMap_t);
            break;
        case pp_Type_sglp_XCreateWindow_t:
            return sizeof(pp_sglp_XCreateWindow_t);
            break;
        case pp_Type_sglp_XMapWindow_t:
            return sizeof(pp_sglp_XMapWindow_t);
            break;
        case pp_Type_sglp_XStoreName_t:
            return sizeof(pp_sglp_XStoreName_t);
            break;
        case pp_Type_sglp_XSelectInput_t:
            return sizeof(pp_sglp_XSelectInput_t);
            break;
        case pp_Type_sglp_XPending_t:
            return sizeof(pp_sglp_XPending_t);
            break;
        case pp_Type_sglp_XNextEvent_t:
            return sizeof(pp_sglp_XNextEvent_t);
            break;
        case pp_Type_sglp_XKeycodeToKeysym_t:
            return sizeof(pp_sglp_XKeycodeToKeysym_t);
            break;
        case pp_Type_sglp_XGetWindowAttributes_t:
            return sizeof(pp_sglp_XGetWindowAttributes_t);
            break;
        case pp_Type_sglp_XQueryPointer_t:
            return sizeof(pp_sglp_XQueryPointer_t);
            break;
        case pp_Type_sglp_sem_open_t:
            return sizeof(pp_sglp_sem_open_t);
            break;
        case pp_Type_sglp_sem_close_t:
            return sizeof(pp_sglp_sem_close_t);
            break;
        case pp_Type_sglp_pthread_create_t:
            return sizeof(pp_sglp_pthread_create_t);
            break;
        case pp_Type_sglp_glXGetProcAddressArb_t:
            return sizeof(pp_sglp_glXGetProcAddressArb_t);
            break;
        case pp_Type_sglp_glXChooseVisual_t:
            return sizeof(pp_sglp_glXChooseVisual_t);
            break;
        case pp_Type_sglp_glXCreateContext_t:
            return sizeof(pp_sglp_glXCreateContext_t);
            break;
        case pp_Type_sglp_glXMakeCurrent_t:
            return sizeof(pp_sglp_glXMakeCurrent_t);
            break;
        case pp_Type_sglp_glXSwapBuffers_t:
            return sizeof(pp_sglp_glXSwapBuffers_t);
            break;
        case pp_Type_sglm_Bool:
            return sizeof(pp_sglm_Bool);
            break;
        case pp_Type_sglp_Key:
            return sizeof(pp_int);
            break;
        case pp_Type_Player_Direction:
            return sizeof(pp_int);
            break;
        case pp_Type_ID:
            return sizeof(pp_int);
            break;
        case pp_Type_sglp_Sprite:
            return sizeof(pp_sglp_Sprite);
            break;
        case pp_Type_sglp_PlayingSound:
            return sizeof(pp_sglp_PlayingSound);
            break;
        case pp_Type_sglp_OpenGlFunctions:
            return sizeof(pp_sglp_OpenGlFunctions);
            break;
        case pp_Type_sglp_Settings:
            return sizeof(pp_sglp_Settings);
            break;
        case pp_Type_sglp_File:
            return sizeof(pp_sglp_File);
            break;
        case pp_Type_sglp_API:
            return sizeof(pp_sglp_API);
            break;
        case pp_Type_sglp_LoadedSound:
            return sizeof(pp_sglp_LoadedSound);
            break;
        case pp_Type_sglp_SoundOutputBuffer:
            return sizeof(pp_sglp_SoundOutputBuffer);
            break;
        case pp_Type_sglp_AudioState:
            return sizeof(pp_sglp_AudioState);
            break;
        case pp_Type_sglp_WAVEHeader:
            return sizeof(pp_sglp_WAVEHeader);
            break;
        case pp_Type_sglp_WavChunk:
            return sizeof(pp_sglp_WavChunk);
            break;
        case pp_Type_sglp_WavFormat:
            return sizeof(pp_sglp_WavFormat);
            break;
        case pp_Type_sglp_RiffIter:
            return sizeof(pp_sglp_RiffIter);
            break;
        case pp_Type_SGLP_XINPUT_GAMEPAD:
            return sizeof(pp_SGLP_XINPUT_GAMEPAD);
            break;
        case pp_Type_SGLP_XINPUT_STATE:
            return sizeof(pp_SGLP_XINPUT_STATE);
            break;
        case pp_Type_SGLP_XINPUT_VIBRATION:
            return sizeof(pp_SGLP_XINPUT_VIBRATION);
            break;
        case pp_Type_sglp_Win32SoundOutput:
            return sizeof(pp_sglp_Win32SoundOutput);
            break;
        case pp_Type_WorkQueueEntry:
            return sizeof(pp_WorkQueueEntry);
            break;
        case pp_Type_WorkQueue:
            return sizeof(pp_WorkQueue);
            break;
        case pp_Type_sglm_V2:
            return sizeof(pp_sglm_V2);
            break;
        case pp_Type_sglm_Mat4x4:
            return sizeof(pp_sglm_Mat4x4);
            break;
        case pp_Type___m128:
            return sizeof(pp___m128);
            break;
        case pp_Type_Entity:
            return sizeof(pp_Entity);
            break;
        case pp_Type_Player:
            return sizeof(pp_Player);
            break;
        case pp_Type_Game_State:
            return sizeof(pp_Game_State);
            break;
        case pp_Type_V2:
            return sizeof(pp_V2);
            break;
        case pp_Type___m128i:
            return sizeof(pp___m128i);
            break;
    }

    PP_ASSERT(0);
    return(0);
}
PP_STATIC char const * pp_enum_to_string(pp_Type type, intptr_t index);
// uintptr_t pp_serialize_struct(TYPE *var, TYPE, buffer, buffer_size);
#define pp_serialize_struct(var, Type, buf, size) pp_serialize_struct_(var, PP_CONCAT(pp_Type_, Type), PP_TO_STRING(var), 0, buf, size, 0)
PP_STATIC uintptr_t
pp_serialize_struct_(void *var, pp_Type type, char const *name, uintptr_t indent, char *buffer, uintptr_t buf_size, uintptr_t bytes_written) {
    char indent_buf[256] = {0};
    uintptr_t i, num_members;

    PP_ASSERT((buffer) && (buf_size > 0)); // Check params.

    // Setup the indent buffer.
    indent += 4;
    for(i = 0; (i < indent); ++i) {indent_buf[i] = ' ';}

    num_members = pp_get_number_of_members(type); PP_ASSERT(num_members != -1); // Get the number of members for the for loop.
    for(i = 0; (i < num_members); ++i) {
        pp_MemberDefinition member = pp_get_members_from_type(type, i);
        uintptr_t *member_ptr = (uintptr_t *)((char *)var + member.offset); // Get the actual pointer to the memory address.
        pp_StructureType struct_type = pp_get_structure_type(member.type);
        if(struct_type == pp_StructureType_primitive) {
            char const *type_as_string = pp_type_to_string(member.type);
            if(member.arr_size > 1) {
                for(int j = 0; (j < member.arr_size); ++j) {
                    uintptr_t *member_ptr_as_uintptr = (uintptr_t *)member_ptr; // For arrays of pointers.
                    pp_MyBool is_null = (member.ptr) ? member_ptr_as_uintptr[j] == 0 : PP_FALSE;
                    if(!is_null) {

#define print_prim_arr(m, Type, p) Type *v = (member.ptr) ? *(Type **)((char unsigned *)member_ptr + (sizeof(uintptr_t) * j)) : &((Type *)member_ptr)[j]; bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s " #Type " %s%s[%d] = " m "", indent_buf, (member.ptr) ? "*" : "", member.name, j, p (Type *)v)
                        pp_Type original_type = pp_typedef_to_original(member.type);
                        if(original_type == pp_Type_double)     { print_prim_arr("%f",  double, *); }
                        else if(original_type == pp_Type_float) { print_prim_arr("%f",  float, *);  }
                        else if(original_type == pp_Type_int)   { print_prim_arr("%d",  int, *);    }
                        else if(original_type == pp_Type_long)  { print_prim_arr("%ld", long, *);   }
                        else if(original_type == pp_Type_short) { print_prim_arr("%d",  short, *);  }

                        else if(original_type == pp_Type_uint64_t) { print_prim_arr("%lu", uint64_t, *); }
                        else if(original_type == pp_Type_int64_t)  { print_prim_arr("%ld", int64_t, *);  }
                        else if(original_type == pp_Type_uint32_t) { print_prim_arr("%u", uint32_t, *); }
                        else if(original_type == pp_Type_int32_t)  { print_prim_arr("%d", int32_t, *);  }
                        else if(original_type == pp_Type_uint16_t) { print_prim_arr("%u", uint16_t, *); }
                        else if(original_type == pp_Type_int16_t)  { print_prim_arr("%d", int16_t, *);  }
                        else if(original_type == pp_Type_uint8_t) { print_prim_arr("%u", uint8_t, *); }
                        else if(original_type == pp_Type_int8_t)  { print_prim_arr("%d", int8_t, *);  }

                        //else if(original_type == pp_Type_intptr_t)  { print_prim_arr("%ld", intptr_t, *);  }
                        //else if(original_type == pp_Type_uintptr_t) { print_prim_arr("%lu", uintptr_t, *); }
                        else if(original_type == pp_Type_size_t)    { print_prim_arr("%zu", size_t, *);    }

                        else if(original_type == pp_Type_char) {
                            if(member.ptr) {
                                print_prim_arr("%s",  char, /**/);
                            } else {
                                print_prim_arr("%c",  char, *);
                            }
                        }
#undef print_prim_arr
                    } else {
                        bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s %s %s%s[%d] = (null)", indent_buf, type_as_string, (member.ptr) ? "*" : "", member.name, j);
                    }
                }
            } else {
                uintptr_t *v = 0;
                if(member.ptr) {
                    v = *(uintptr_t **)member_ptr;
                    for(uintptr_t k = 0; (k < member.ptr - 1); ++k) {
                        v = *(uintptr_t **)v;
                    }
                } else {
                    v = (uintptr_t *)member_ptr;
                }
                if(v) {
#define print_prim(m, Type, p) bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s " #Type " %s%s = " m "", indent_buf, (member.ptr) ? "*" : "", member.name, p (Type *)v)
                    pp_Type original_type = pp_typedef_to_original(member.type);
                    if(original_type == pp_Type_double)     { print_prim("%f",  double, *); }
                    else if(original_type == pp_Type_float) { print_prim("%f",  float, *);  }
                    else if(original_type == pp_Type_int)   { print_prim("%d",  int, *);    }
                    else if(original_type == pp_Type_long)  { print_prim("%ld", long, *);   }
                    else if(original_type == pp_Type_short) { print_prim("%d",  short, *);  }

                    else if(original_type == pp_Type_uint64_t)  { print_prim("%lu", uint64_t, *); }
                    else if(original_type == pp_Type_int64_t)   { print_prim("%ld", int64_t, *);  }
                    else if(original_type == pp_Type_uint32_t)  { print_prim("%u", uint32_t, *); }
                    else if(original_type == pp_Type_int32_t)   { print_prim("%d", int32_t, *);  }
                    else if(original_type == pp_Type_uint16_t)  { print_prim("%u", uint16_t, *); }
                    else if(original_type == pp_Type_int16_t)   { print_prim("%d", int16_t, *);  }
                    else if(original_type == pp_Type_uint8_t)   { print_prim("%u", uint8_t, *);  }
                    else if(original_type == pp_Type_int8_t)    { print_prim("%d", int8_t, *);   }

                    //else if(original_type == pp_Type_intptr_t)  { print_prim("%ld", intptr_t, *);  }
                    //else if(original_type == pp_Type_uintptr_t) { print_prim("%lu", uintptr_t, *); }
                    else if(original_type == pp_Type_size_t)    { print_prim("%zu", size_t, *);    }

                    else if(original_type == pp_Type_char) {
                        if(member.ptr) {
                            print_prim("%s",  char, /**/);
                        } else {
                            print_prim("%c",  char, *);
                        }
                    }
#undef print_prim
                } else {
                    bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s %s *%s = (null)", indent_buf, type_as_string, member.name);
                }
            }
        } else if(struct_type == pp_StructureType_enum) {
            char const *type_as_string = pp_type_to_string(member.type);

            int *v = 0; // Assuming the enum is an integer.
            if(member.ptr) {
                v = *(int **)member_ptr;
                for(int k = 0; (k < member.ptr - 1); ++k) {
                    v = *(int **)v;
                }
            } else {
                v = (int *)member_ptr;
            }

            pp_Type original_type = pp_typedef_to_original(member.type);
            char const *enum_as_string = pp_enum_to_string(original_type, *(int *)v);
            if(enum_as_string) {
                bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s %s *%s = %s", indent_buf, type_as_string, member.name, enum_as_string);
            }

        } else if(struct_type == pp_StructureType_struct) {
            char tmp_arr_buf[64] = {0};
            if(member.arr_size > 1) {
                PP_SPRINTF(tmp_arr_buf, 32, "[%d]", (int)member.arr_size);
            }

            bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s%s %s%s%s", indent_buf, pp_type_to_string(member.type), (member.ptr) ? "*" : "",member.name, tmp_arr_buf);
            if(member.arr_size <= 1) {
                void *ptr = (member.ptr) ? *(uintptr_t **)member_ptr : member_ptr;
                bytes_written = pp_serialize_struct_(ptr, member.type, member.name, indent, buffer, buf_size - bytes_written, bytes_written);
            } else {
                for(uintptr_t j = 0; (j < member.arr_size); ++j) {
                    uintptr_t size_of_struct = pp_get_size_from_type(member.type);

                    char unsigned *ptr = 0;
                    if(member.ptr) {
                        ptr = *(char unsigned **)((char unsigned *)member_ptr + (j * sizeof(uintptr_t)));
                    } else {
                        ptr = ((char unsigned *)member_ptr + (j * size_of_struct));
                    }

                    bytes_written = pp_serialize_struct_(ptr, member.type, member.name, indent, buffer, buf_size - bytes_written, bytes_written);
                }
            }
        }
    }

    buffer[bytes_written] = 0;
    return(bytes_written);
}

//
// Number of members in an enum.
//

//
// Number of members in an enum.
//
PP_STATIC uintptr_t pp_get_enum_size_from_type(pp_Type type) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type_sglp_Key: { return(55); } break;
        case pp_Type_Player_Direction: { return(4); } break;
        case pp_Type_ID: { return(6); } break;
    }

    PP_ASSERT(0);
    return(0);
}

//
// String to enum.
//
PP_STATIC intptr_t pp_string_to_enum(pp_Type type, char const *str) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type_sglp_Key: {
            if(pp_string_compare(str, "sglp_key_unknown")) { return(0); }
            else if(pp_string_compare(str, "sglp_key_ctrl")) { return(1); }
            else if(pp_string_compare(str, "sglp_key_shift")) { return(2); }
            else if(pp_string_compare(str, "sglp_key_alt")) { return(3); }
            else if(pp_string_compare(str, "sglp_key_space")) { return(4); }
            else if(pp_string_compare(str, "sglp_key_escape")) { return(5); }
            else if(pp_string_compare(str, "sglp_key_left")) { return(6); }
            else if(pp_string_compare(str, "sglp_key_right")) { return(7); }
            else if(pp_string_compare(str, "sglp_key_up")) { return(8); }
            else if(pp_string_compare(str, "sglp_key_down")) { return(9); }
            else if(pp_string_compare(str, "sglp_left_stick_x")) { return(10); }
            else if(pp_string_compare(str, "sglp_left_stick_y")) { return(11); }
            else if(pp_string_compare(str, "sglp_right_stick_x")) { return(12); }
            else if(pp_string_compare(str, "sglp_right_stick_y")) { return(13); }
            else if(pp_string_compare(str, "sglp_dpad_right")) { return(14); }
            else if(pp_string_compare(str, "sglp_dpad_up")) { return(15); }
            else if(pp_string_compare(str, "sglp_dpad_down")) { return(16); }
            else if(pp_string_compare(str, "sglp_dpad_left")) { return(17); }
            else if(pp_string_compare(str, "start")) { return(18); }
            else if(pp_string_compare(str, "back")) { return(19); }
            else if(pp_string_compare(str, "left_shoulder")) { return(20); }
            else if(pp_string_compare(str, "right_shoulder")) { return(21); }
            else if(pp_string_compare(str, "left_thumb")) { return(22); }
            else if(pp_string_compare(str, "right_thumb")) { return(23); }
            else if(pp_string_compare(str, "sglp_controller_a")) { return(24); }
            else if(pp_string_compare(str, "sglp_controller_b")) { return(25); }
            else if(pp_string_compare(str, "sglp_controller_x")) { return(26); }
            else if(pp_string_compare(str, "sglp_controller_y")) { return(27); }
            else if(pp_string_compare(str, "sglp_key_a")) { return(65); }
            else if(pp_string_compare(str, "sglp_key_b")) { return(66); }
            else if(pp_string_compare(str, "sglp_key_c")) { return(67); }
            else if(pp_string_compare(str, "sglp_key_d")) { return(68); }
            else if(pp_string_compare(str, "sglp_key_e")) { return(69); }
            else if(pp_string_compare(str, "sglp_key_f")) { return(70); }
            else if(pp_string_compare(str, "sglp_key_g")) { return(71); }
            else if(pp_string_compare(str, "sglp_key_h")) { return(72); }
            else if(pp_string_compare(str, "sglp_key_i")) { return(73); }
            else if(pp_string_compare(str, "sglp_key_j")) { return(74); }
            else if(pp_string_compare(str, "sglp_key_k")) { return(75); }
            else if(pp_string_compare(str, "sglp_key_l")) { return(76); }
            else if(pp_string_compare(str, "sglp_key_m")) { return(77); }
            else if(pp_string_compare(str, "sglp_key_n")) { return(78); }
            else if(pp_string_compare(str, "sglp_key_o")) { return(79); }
            else if(pp_string_compare(str, "sglp_key_p")) { return(80); }
            else if(pp_string_compare(str, "sglp_key_q")) { return(81); }
            else if(pp_string_compare(str, "sglp_key_r")) { return(82); }
            else if(pp_string_compare(str, "sglp_key_s")) { return(83); }
            else if(pp_string_compare(str, "sglp_key_t")) { return(84); }
            else if(pp_string_compare(str, "sglp_key_u")) { return(85); }
            else if(pp_string_compare(str, "sglp_key_v")) { return(86); }
            else if(pp_string_compare(str, "sglp_key_w")) { return(87); }
            else if(pp_string_compare(str, "sglp_key_x")) { return(88); }
            else if(pp_string_compare(str, "sglp_key_y")) { return(89); }
            else if(pp_string_compare(str, "sglp_key_z")) { return(90); }
            else if(pp_string_compare(str, "sglp_key_cnt")) { return(128); }
        } break;
        case pp_Type_Player_Direction: {
            if(pp_string_compare(str, "Player_Direction_left")) { return(0); }
            else if(pp_string_compare(str, "Player_Direction_right")) { return(2); }
            else if(pp_string_compare(str, "Player_Direction_up")) { return(4); }
            else if(pp_string_compare(str, "Player_Direction_down")) { return(6); }
        } break;
        case pp_Type_ID: {
            if(pp_string_compare(str, "ID_unknown")) { return(0); }
            else if(pp_string_compare(str, "ID_sound_bloop")) { return(1); }
            else if(pp_string_compare(str, "ID_sound_background")) { return(2); }
            else if(pp_string_compare(str, "ID_sprite_player")) { return(3); }
            else if(pp_string_compare(str, "ID_sprite_enemy_one")) { return(4); }
            else if(pp_string_compare(str, "ID_sprite_bitmap_font")) { return(5); }
        } break;
    }

    PP_ASSERT(0);
    return(0);
}

//
// Enum to string.
//
PP_STATIC char const * pp_enum_to_string(pp_Type type, intptr_t index) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type_sglp_Key: {
            switch(index) {
                case 0: { return("sglp_key_unknown"); } break;
                case 1: { return("sglp_key_ctrl"); } break;
                case 2: { return("sglp_key_shift"); } break;
                case 3: { return("sglp_key_alt"); } break;
                case 4: { return("sglp_key_space"); } break;
                case 5: { return("sglp_key_escape"); } break;
                case 6: { return("sglp_key_left"); } break;
                case 7: { return("sglp_key_right"); } break;
                case 8: { return("sglp_key_up"); } break;
                case 9: { return("sglp_key_down"); } break;
                case 10: { return("sglp_left_stick_x"); } break;
                case 11: { return("sglp_left_stick_y"); } break;
                case 12: { return("sglp_right_stick_x"); } break;
                case 13: { return("sglp_right_stick_y"); } break;
                case 14: { return("sglp_dpad_right"); } break;
                case 15: { return("sglp_dpad_up"); } break;
                case 16: { return("sglp_dpad_down"); } break;
                case 17: { return("sglp_dpad_left"); } break;
                case 18: { return("start"); } break;
                case 19: { return("back"); } break;
                case 20: { return("left_shoulder"); } break;
                case 21: { return("right_shoulder"); } break;
                case 22: { return("left_thumb"); } break;
                case 23: { return("right_thumb"); } break;
                case 24: { return("sglp_controller_a"); } break;
                case 25: { return("sglp_controller_b"); } break;
                case 26: { return("sglp_controller_x"); } break;
                case 27: { return("sglp_controller_y"); } break;
                case 65: { return("sglp_key_a"); } break;
                case 66: { return("sglp_key_b"); } break;
                case 67: { return("sglp_key_c"); } break;
                case 68: { return("sglp_key_d"); } break;
                case 69: { return("sglp_key_e"); } break;
                case 70: { return("sglp_key_f"); } break;
                case 71: { return("sglp_key_g"); } break;
                case 72: { return("sglp_key_h"); } break;
                case 73: { return("sglp_key_i"); } break;
                case 74: { return("sglp_key_j"); } break;
                case 75: { return("sglp_key_k"); } break;
                case 76: { return("sglp_key_l"); } break;
                case 77: { return("sglp_key_m"); } break;
                case 78: { return("sglp_key_n"); } break;
                case 79: { return("sglp_key_o"); } break;
                case 80: { return("sglp_key_p"); } break;
                case 81: { return("sglp_key_q"); } break;
                case 82: { return("sglp_key_r"); } break;
                case 83: { return("sglp_key_s"); } break;
                case 84: { return("sglp_key_t"); } break;
                case 85: { return("sglp_key_u"); } break;
                case 86: { return("sglp_key_v"); } break;
                case 87: { return("sglp_key_w"); } break;
                case 88: { return("sglp_key_x"); } break;
                case 89: { return("sglp_key_y"); } break;
                case 90: { return("sglp_key_z"); } break;
                case 128: { return("sglp_key_cnt"); } break;
            }
        } break;
        case pp_Type_Player_Direction: {
            switch(index) {
                case 0: { return("Player_Direction_left"); } break;
                case 2: { return("Player_Direction_right"); } break;
                case 4: { return("Player_Direction_up"); } break;
                case 6: { return("Player_Direction_down"); } break;
            }
        } break;
        case pp_Type_ID: {
            switch(index) {
                case 0: { return("ID_unknown"); } break;
                case 1: { return("ID_sound_bloop"); } break;
                case 2: { return("ID_sound_background"); } break;
                case 3: { return("ID_sprite_player"); } break;
                case 4: { return("ID_sprite_enemy_one"); } break;
                case 5: { return("ID_sprite_bitmap_font"); } break;
            }
        } break;
    }

    PP_ASSERT(0);
    return(0);
}

#define PP_GENERATED_H
#endif // #if defined(PP_GENERATED_H)

