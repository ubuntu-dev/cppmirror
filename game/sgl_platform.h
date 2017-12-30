/*
    sgl_platform - v0.1 - public domain platform layer - no warranty implied; use at your own risk

    Write #define SGLP_IMPLEMENTATION in ONE of the C/C++ files to create the implementation.

    // It should look like this.
    #define SGLP_IMPLEMENTATION
    #include "sgl_platform.h"

    Some of the "functions" (macros) in this file can be redefined by the user. These include
    - SGLP_ASSERT(x)

    // To do this just do the following in the file that defines SGLP_IMPLEMENTATION
    #define SGLP_ASSERT(x) my_assert(x)

    LICENSE at end of file.
*/


//
// Header file
//

#if !defined(_SGL_PLATFORM_H)

#define SGLP_COMPILER_MSVC 0
#define SGLP_COMPILER_CLANG 0
#define SGLP_COMPILER_GCC 0

#define SGLP_ENVIRONMENT64 1
#define SGLP_ENVIRONMENT32 0

#define SGLP_OS_WIN32 0
#define SGLP_OS_LINUX 0

#if defined(__clang__)
    #undef SGLP_COMPILER_CLANG
    #define SGLP_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #undef SGLP_COMPILER_MSVC
    #define SGLP_COMPILER_MSVC 1
#elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.
    #undef SGLP_COMPILER_GCC
    #define SGLP_COMPILER_GCC 1
#else
    #error "Could not detect compiler."
#endif

#if defined(__linux__)
    #undef SGLP_OS_LINUX
    #define SGLP_OS_LINUX 1
#elif defined(_WIN32)
    #undef SGLP_OS_WIN32
    #define SGLP_OS_WIN32 1
#else
    #error "Could not detect OS"
#endif

#if SGLP_OS_LINUX
    #if (__x86_64__ || __ppc64__)
        #undef SGLP_ENVIRONMENT64
        #define SGLP_ENVIRONMENT64 1
    #else
        #undef SGLP_ENVIRONMENT32
        #define SGLP_ENVIRONMENT32 1
    #endif
#elif SGLP_OS_WIN32
    #if defined(_WIN64)
        #undef SGLP_ENVIRONMENT64
        #define SGLP_ENVIRONMENT64 1
    #else
        #undef SGLP_ENVIRONMENT32
        #define SGLP_ENVIRONMENT32 1
    #endif
#endif


// stdcall
#define SGLP_STDCALL
#if SGLP_COMPILER_MSVC
    #undef SGLP_STDCALL
    #define SGLP_STDCALL __stdcall
#endif

#include <stdint.h>

typedef int sglp_Bool;
#define SGLP_TRUE 1
#define SGLP_FALSE 0

//
// Sprites.
//
typedef struct sglp_Sprite {
    int32_t w, h;
    int32_t id;
    int32_t frame_cnt_x, frame_cnt_y;
    uint32_t vbo_arr[2];
    uint32_t mesh;
} sglp_Sprite;

struct sglp_API;
sglp_Sprite sglp_load_image(struct sglp_API *api, uint8_t *img_data, int32_t frame_cnt_x, int32_t frame_cnt_y, int32_t id,
                            int32_t width, int32_t height, int32_t no_components);
void sglp_clear_screen_for_frame(void);
void sglp_draw_sprite(sglp_Sprite sprite, int32_t cur_frame, float const *tform);
void sglp_draw_sprite_frame_matrix(sglp_Sprite sprite, int32_t cur_frame_x, int32_t cur_frame_y, float const *tform);
void sglp_draw_black_box(float const *tform);
uint32_t sglp_load_and_compile_shaders(char const *fvertex, char const *ffragment);

//
// Audio.
//
typedef struct sglp_PlayingSound {
    struct sglp_PlayingSound *next;
    float cur_volume0, cur_volume1;
    float target_volume0, target_volume1;
    float dcur_volume0, dcur_volume1;

    float dsample;

    int32_t id;
    float samples_played;
} sglp_PlayingSound;

sglp_PlayingSound *sglp_get_playing_sound(int32_t id);
sglp_PlayingSound *sglp_play_new_audio(struct sglp_API *api, int32_t id);
sglp_PlayingSound *sglp_play_audio(struct sglp_API *api, int32_t id);
void sglp_change_volume(sglp_PlayingSound *snd, float fade_duration_seconds, float vol0, float vol1);
void sglp_change_pitch(sglp_PlayingSound *snd, float dsample);

sglp_Bool sglp_load_wav(struct sglp_API *api, int32_t id, void *e, uintptr_t size);

//
// Keyboard.
//
enum sglp_Key {
    sglp_key_unknown,

    // TODO(Jonny): Add all the rest of the keys.
    sglp_key_ctrl,
    sglp_key_shift,
    sglp_key_alt,
    sglp_key_space,
    sglp_key_escape,

    sglp_key_left,
    sglp_key_right,
    sglp_key_up,
    sglp_key_down,

    sglp_left_stick_x,
    sglp_left_stick_y,

    sglp_right_stick_x,
    sglp_right_stick_y,

    sglp_dpad_right,
    sglp_dpad_up,
    sglp_dpad_down,
    sglp_dpad_left,

    start,
    back,

    left_shoulder,
    right_shoulder,
    left_thumb,
    right_thumb,

    sglp_controller_a,
    sglp_controller_b,
    sglp_controller_x,
    sglp_controller_y,

    sglp_key_a = 65, // A
    sglp_key_b,
    sglp_key_c,
    sglp_key_d,
    sglp_key_e,
    sglp_key_f,
    sglp_key_g,
    sglp_key_h,
    sglp_key_i,
    sglp_key_j,
    sglp_key_k,
    sglp_key_l,
    sglp_key_m,
    sglp_key_n,
    sglp_key_o,
    sglp_key_p,
    sglp_key_q,
    sglp_key_r,
    sglp_key_s,
    sglp_key_t,
    sglp_key_u,
    sglp_key_v,
    sglp_key_w,
    sglp_key_x,
    sglp_key_y,
    sglp_key_z,

    sglp_key_cnt = 128
};

//
// OpenGL
//
typedef uint32_t   sglp_GLenum;
typedef uint32_t   sglp_GLbitfield;
typedef uint32_t   sglp_GLuint;
typedef int            sglp_GLint;
typedef int            sglp_GLsizei;
typedef uint8_t  sglp_GLboolean;
typedef char    sglp_GLbyte;
typedef short          sglp_GLshort;
typedef uint8_t  sglp_GLubyte;
typedef uint16_t sglp_GLushort;
typedef uint32_t  sglp_GLulong;
typedef float          sglp_GLfloat;
typedef float          sglp_GLclampf;
typedef double         sglp_GLdouble;
typedef double         sglp_GLclampd;
typedef void           sglp_GLvoid;
typedef char           sglp_GLchar;
typedef long           sglp_GLsizeiptr; // TODO(Jonny): Is this right?

#define SGLP_GL_FALSE              0x0
#define SGLP_GL_TEXTURE_2D         0x0DE1
#define SGLP_GL_CLAMP              0x2900
#define SGLP_GL_VERSION            0x1F02
#define SGLP_GL_TEXTURE_MIN_FILTER 0x2801
#define SGLP_GL_LINEAR             0x2601
#define SGLP_GL_NEAREST            0x2600
#define SGLP_GL_TEXTURE_MAG_FILTER 0x2800
#define SGLP_GL_RGB                0x1907
#define SGLP_GL_RGBA               0x1908
#define SGLP_GL_UNSIGNED_BYTE      0x1401
#define SGLP_GL_FLOAT              0x1406
#define SGLP_GL_TEXTURE_WRAP_S     0x2802
#define SGLP_GL_TEXTURE_WRAP_T     0x2803
#define SGLP_GL_REPEAT             0x2901
#define SGLP_GL_TRIANGLE_STRIP     0x0005
#define SGLP_GL_COLOR_BUFFER_BIT   0x00004000
#define SGLP_GL_ALPHA              0x1906
#define SGLP_GL_QUADS              0x0007
#define SGLP_GL_NO_ERROR           0x0
#define SGLP_GL_INVALID_ENUM       0x0500
#define SGLP_GL_INVALID_VALUE      0x0501
#define SGLP_GL_INVALID_OPERATION  0x0502
#define SGLP_GL_STACK_OVERFLOW     0x0503
#define SGLP_GL_STACK_UNDERFLOW    0x0504
#define SGLP_GL_OUT_OF_MEMORY      0x0505
#define SGLP_GL_FRAGMENT_SHADER    0x8B30
#define SGLP_GL_VERTEX_SHADER      0x8B31
#define SGLP_GL_COMPILE_STATUS     0x8B81
#define SGLP_GL_ARRAY_BUFFER       0x8892
#define SGLP_GL_STATIC_DRAW        0x88E4
#define SGLP_GL_TEXTURE_WRAP_R     0x8072
#define SGLP_GL_INFO_LOG_LENGTH    0x8B84
#define SGLP_GL_TEXTURE_MIN_LOD    0x813A
#define SGLP_GL_TEXTURE_MAX_LOD    0x813B
#define SGLP_GL_TEXTURE_BASE_LEVEL 0x813C
#define SGLP_GL_TEXTURE_MAX_LEVEL  0x813D

// OpenGL 1
typedef void SGLP_STDCALL sglp_glBindTexture_t(sglp_GLenum target, unsigned int texture);
typedef void SGLP_STDCALL sglp_glClear_t(sglp_GLbitfield mask);
typedef void SGLP_STDCALL sglp_glClearColor_t(sglp_GLclampf red, sglp_GLclampf green, sglp_GLclampf blue, sglp_GLclampf alpha);
typedef void SGLP_STDCALL sglp_glDrawArrays_t(sglp_GLenum mode, int first, sglp_GLsizei count);
typedef sglp_GLenum SGLP_STDCALL sglp_glGetError_t(void);
typedef void SGLP_STDCALL sglp_glTexImage2D_t(sglp_GLenum target, int level, int internalFormat, sglp_GLsizei width, sglp_GLsizei height, int border, sglp_GLenum format, sglp_GLenum type, const sglp_GLvoid *data);
typedef void SGLP_STDCALL sglp_glTexParameteri_t(sglp_GLenum target, sglp_GLenum pname, int param);
typedef const sglp_GLubyte *SGLP_STDCALL sglp_glGetString_t(sglp_GLenum name);
typedef void SGLP_STDCALL sglp_glViewport_t(sglp_GLint x, sglp_GLint y, sglp_GLsizei width, sglp_GLsizei height);

// OpenGL 2
typedef unsigned int SGLP_STDCALL sglp_glCreateShader_t(sglp_GLenum shaderType);
typedef void SGLP_STDCALL sglp_glShaderSource_t(unsigned int shader, sglp_GLsizei count, const sglp_GLchar * *string, const int *length);
typedef void SGLP_STDCALL sglp_glCompileShader_t(unsigned int shader);
typedef void SGLP_STDCALL sglp_glGetShaderiv_t(unsigned int shader, sglp_GLenum pname, int *params);
typedef unsigned int SGLP_STDCALL sglp_glCreateProgram_t();
typedef void SGLP_STDCALL sglp_glAttachShader_t(unsigned int program, unsigned int shader);
typedef void SGLP_STDCALL sglp_glBindAttribLocation_t(unsigned int program, unsigned int index, const sglp_GLchar *name);
typedef void SGLP_STDCALL sglp_glLinkProgram_t(unsigned int program);
typedef void SGLP_STDCALL sglp_glUseProgram_t(unsigned int program);
typedef void SGLP_STDCALL sglp_glGenBuffers_t(sglp_GLsizei n, unsigned int *buffers);
typedef void SGLP_STDCALL sglp_glBindBuffer_t(sglp_GLenum target, unsigned int buffer);
typedef void SGLP_STDCALL sglp_glBufferData_t(sglp_GLenum target, sglp_GLsizeiptr size, const sglp_GLvoid *data, sglp_GLenum usage);
typedef void SGLP_STDCALL sglp_glVertexAttribPointer_t(unsigned int index, int size, sglp_GLenum type, sglp_GLboolean normalized, sglp_GLsizei stride, const sglp_GLvoid *pointers);
typedef void SGLP_STDCALL sglp_glEnableVertexAttribArray_t(unsigned int index);
typedef void SGLP_STDCALL sglp_glUniform1i_t(int location, sglp_GLint v0);
typedef void SGLP_STDCALL sglp_glUniform2f_t(int location, sglp_GLfloat v0, sglp_GLfloat v1);
typedef void SGLP_STDCALL sglp_glUniform4f_t(int location, sglp_GLfloat v0, sglp_GLfloat v1, sglp_GLfloat v2, sglp_GLfloat v3);
typedef void SGLP_STDCALL sglp_glUniformMatrix4fv_t(int location, sglp_GLsizei count, sglp_GLboolean transpose, const sglp_GLfloat *value);
typedef int SGLP_STDCALL sglp_glGetUniformLocation_t(unsigned int program, const sglp_GLchar *name);
typedef sglp_GLboolean SGLP_STDCALL sglp_glIsShader_t(unsigned int shader);
typedef void SGLP_STDCALL sglp_glGetProgramiv_t(unsigned int program, sglp_GLenum pname, int *params);
typedef void SGLP_STDCALL sglp_glGetProgramInfoLog_t(unsigned int shader, sglp_GLsizei maxLength, sglp_GLsizei *length, sglp_GLchar *infoLog);
typedef void SGLP_STDCALL sglp_glGetShaderInfoLog_t(unsigned int shader, sglp_GLsizei maxLength, sglp_GLsizei *length, sglp_GLchar *infoLog);

// OpenGL 3
typedef void SGLP_STDCALL sglp_glGenVertexArrays_t(sglp_GLsizei n, unsigned int *arrays);
typedef void SGLP_STDCALL sglp_glBindVertexArray_t(unsigned int Array);

typedef struct sglp_OpenGlFunctions {
    sglp_glBindTexture_t             *glBindTexture;
    sglp_glClear_t                   *glClear;
    sglp_glClearColor_t              *glClearColor;
    sglp_glDrawArrays_t              *glDrawArrays;
    sglp_glGetError_t                *glGetError;
    sglp_glTexImage2D_t              *glTexImage2D;
    sglp_glTexParameteri_t           *glTexParameteri;
    sglp_glGetString_t               *glGetString;
    sglp_glViewport_t                *glViewport;

    sglp_glCreateShader_t            *glCreateShader;
    sglp_glShaderSource_t            *glShaderSource;
    sglp_glCompileShader_t           *glCompileShader;
    sglp_glGetShaderiv_t             *glGetShaderiv;
    sglp_glCreateProgram_t           *glCreateProgram;
    sglp_glAttachShader_t            *glAttachShader;
    sglp_glBindAttribLocation_t      *glBindAttribLocation;
    sglp_glLinkProgram_t             *glLinkProgram;
    sglp_glUseProgram_t              *glUseProgram;
    sglp_glGenBuffers_t              *glGenBuffers;
    sglp_glBindBuffer_t              *glBindBuffer;
    sglp_glBufferData_t              *glBufferData;
    sglp_glVertexAttribPointer_t     *glVertexAttribPointer;
    sglp_glEnableVertexAttribArray_t *glEnableVertexAttribArray;
    sglp_glUniform1i_t               *glUniform1i;
    sglp_glUniform2f_t               *glUniform2f;
    sglp_glUniform4f_t               *glUniform4f;
    sglp_glUniformMatrix4fv_t        *glUniformMatrix4fv;
    sglp_glGetUniformLocation_t      *glGetUniformLocation;
    sglp_glIsShader_t                *glIsShader;
    sglp_glGetProgramiv_t              *glGetProgramiv;
    sglp_glGetProgramInfoLog_t       *glGetProgramInfoLog;
    sglp_glGetShaderInfoLog_t        *glGetShaderInfoLog;

    sglp_glGenVertexArrays_t         *glGenVertexArrays;
    sglp_glBindVertexArray_t         *glBindVertexArray;
} sglp_OpenGlFunctions;

//
// Screen coordinate converters.
//
// TODO(Jonny): Take the sglp_API here instead of a specific width/height?
int32_t sglp_gl_coord_to_screen_pixel_x(float glx, int32_t win_width);
int32_t sglp_gl_coord_to_screen_pixel_y(float gly, int32_t win_height);

float sglp_screen_pixel_coord_to_gl_x(int32_t winx, int32_t win_width);
float sglp_screen_pixel_coord_to_gl_y(int32_t winy, int32_t win_height);

float sglp_gl_coord_to_screen_x(float glx);
float sglp_gl_coord_to_screen_y(float gly);

float sglp_screen_to_gl_x(float x);
float sglp_screen_to_gl_y(float y);

//
// Settings.
typedef struct sglp_Settings {
    sglp_Bool fullscreen;
    int32_t win_width, win_height;
    int32_t frame_rate;
    uintptr_t permanent_memory_size;
    int32_t max_no_of_sounds;
    char const *window_title;
    int32_t thread_cnt;
} sglp_Settings;

typedef struct sglp_File {
    uint8_t *e;
    uintptr_t size;
} sglp_File;

typedef struct sglp_API {
    sglp_Settings settings;
    sglp_OpenGlFunctions gl;
    void *permanent_memory;
    float key[256];
    float dt;
    sglp_Bool init_game;
    sglp_Bool quit;

    //
    // Platform
    //

    // sglp_File IO.
    void (*free_file)(struct sglp_API *, sglp_File *);                          // void sglp_free_file(sglp_API *api, sglp_File *file);
    sglp_File (*read_file)(struct sglp_API *, char const *);                    // sglp_File read_file(sglp_API *api, char const *fname);
    sglp_File (*read_file_and_null_terminate)(struct sglp_API *, char const *); // sglp_File read_file_and_null_terminate(sglp_API *api, char const *fname);

    // Processor timestamp.
    uint64_t (*get_processor_timestamp)(void); // uint64_t get_processor_timestamp();

    // Threading.
    void (*add_work_queue_entry)(void *e, void (*callback)(void *data)); // void add_work_queue_entry(void *e, void (*callback)(void *data)));
    void (*complete_all_work)();                                         // void complete_all_work();

    // Memory Allocation.
    void *(*os_malloc)(uintptr_t size);             // void *platform_malloc(uintptr_t size);
    void *(*os_realloc)(void *ptr, uintptr_t size); // void *platform_realloc(void *ptr, uintptr_t size);
    void (*os_free)(void *ptr);                     // void platform_free(void *ptr);
} sglp_API;

//
// External functions that the platform requires be implemented.
//

//
// This function is called before the platform layer has done anything. It is used to setup some information that is passed back.
void sglp_platform_setup_settings_callback(sglp_Settings *settings);

//
// This is the main render loop function. It is called once-per-frame.
void sglp_platform_update_and_render_callback(sglp_API *api);


//
// Source file
//
#if defined(SGLP_IMPLEMENTATION)

#if !defined(SGLP_ASSERT)
    #if SGLP_COMPILER_MSVC
        #define SGLP_ASSERT(x) do { if(!(x)) { __debugbreak(); } } while(0)
    #else
        #define SGLP_ASSERT(x) do { if(!(x)) { *(uintptr_t volatile *)0 = 0; } } while(0) // TODO(Jonny): Find out what debugbreak is actually on Linux.
    #endif
#endif

//
// Utils.
//
#define SGLP_ALIGN_POW2(v, align) ((v + ((align) - 1)) & ~((align) - 1))
#define SGLP_ALIGN4(v)            SGLP_ALIGN_POW2(v, 4)
#define SGLP_ALIGN8(v)            SGLP_ALIGN_POW2(v, 8)
#define SGLP_ALIGN16(v)           SGLP_ALIGN_POW2(v, 16)
#define SGLP_ALIGN32(v)           SGLP_ALIGN_POW2(v, 32)

#define SGLP_BYTES(v)     ((v)                 * (8LL))
#define SGLP_KILOBYTES(v) ((v)                 * (1024LL))
#define SGLP_MEGABYTES(v) ((SGLP_KILOBYTES(v)) * (1024LL))
#define SGLP_GIGABYTES(v) ((SGLP_MEGABYTES(v)) * (1024LL))

#define SGLP_ARRAY_COUNT(arr) (sizeof(arr) / (sizeof(*(arr))))

static sglp_OpenGlFunctions *sglp_global_opengl; // TODO(Jonny): Get rid of this, and require gl functions pass in the sglp_API struct?

static uint32_t sglp_global_uniform_sprite_offset;
static uint32_t sglp_global_uniform_pos;
static uint32_t sglp_global_uniform_colour;
static uint32_t sglp_global_uniform_no_texture;

static void sglp_memcpy(void *dest, void *src, uintptr_t cnt) {
    int i;
    uint8_t *dest8 = (uint8_t *)dest;
    uint8_t *src8  = (uint8_t *)src;
    for(i = 0; (i < cnt); ++i) {
        dest8[i] = src8[i];
    }
}

static void sglp_memset(void *dest, uint8_t x, uintptr_t size) {
    int i;
    uint8_t *dest8 = (uint8_t *)dest;
    for(i = 0; (i < size); ++i) {
        dest8[i] = x;
    }
}

int32_t sglp_gl_coord_to_screen_pixel_x(float glx, int32_t win_width) {
    int32_t res = ((glx + 1) * win_width) / 2;

    return(res);
}

int32_t sglp_gl_coord_to_screen_pixel_y(float gly, int32_t win_height) {
    int32_t res = ((gly + 1) * win_height) / 2;

    return(res);
}

#define SGLP_NORMALISE(v, max, min) (((v) - (min)) / ((max) - (min)))

float sglp_screen_pixel_coord_to_gl_x(int32_t winx, int32_t win_width) {
    float res = SGLP_NORMALISE(winx, win_width / 2, 0) - 1;

    return(res);
}

float sglp_screen_pixel_coord_to_gl_y(int32_t winy, int32_t win_height) {
    float res = SGLP_NORMALISE(winy, win_height / 2, 0) - 1;

    return(res);
}

float sglp_gl_coord_to_screen_x(float glx) {
    float res = (glx + 1.0f) / 2.0f;

    return(res);
}

float sglp_gl_coord_to_screen_y(float gly) {
    float res = (gly + 1.0f) / 2.0f;

    return(res);
}

float sglp_screen_to_gl_x(float x) {
    float res = (x * 2.0f) - 1.0f;

    return(res);
}

float sglp_screen_to_gl_y(float y) {
    float res = (y * 2.0f) - 1.0f;

    return(res);
}

static void sglp_print_shader_err(uint32_t shader) {
    int32_t max_len, msg_len;
    char msg[4096] = {0};

    SGLP_ASSERT(sglp_global_opengl->glIsShader(shader));

    sglp_global_opengl->glGetShaderiv(shader, SGLP_GL_INFO_LOG_LENGTH, &max_len);

    sglp_global_opengl->glGetShaderInfoLog(shader, max_len, &msg_len, msg);

    SGLP_ASSERT(0); // TODO(Jonny): Print the message properly.
}

void sglp_setup_default_shader(void) {
    char const *frag =
        "#version 130\n"
        "\n"
        "// NOTE(Jonny): Some drivers require the following (apparently...)\n"
        "precision highp float;\n"
        "\n"
        "uniform sampler2D uniform_tex_unit;\n"
        "uniform int uniform_bool_no_texture;\n"
        "uniform vec4 sglp_global_uniform_colour;\n"
        "\n"
        "in vec2 out_TexCoord;\n"
        "\n"
        "out vec4 out_Color;\n"
        "\n"
        "void main() {\n"
        "    out_Color = texture2D(uniform_tex_unit, out_TexCoord) * sglp_global_uniform_colour;\n"
        "    if(uniform_bool_no_texture != 0) {\n"
        "        out_Color = sglp_global_uniform_colour;\n"
        "    }\n"
        "\n"
        "    if (out_Color[3] < 0.1) {\n"
        "        discard;\n"
        "    }\n"
        "\n"
        "    // TODO(Jonny): Allow things to be partically transparent...\n"
        "}\n";

    char const *vert =
        "#version 130\n"
        "\n"
        "uniform mat4 sglp_global_uniform_pos;\n"
        "uniform vec2 uniform_offset;\n"
        "\n"
        "in vec3 in_Position;\n"
        "in vec2 in_TexCoord;\n"
        "\n"
        "out vec2 out_TexCoord;\n"
        "\n"
        "void main() {\n"
        "    out_TexCoord = in_TexCoord - uniform_offset;\n"
        "\n"
        "    gl_Position = sglp_global_uniform_pos * vec4(in_Position, 1.0);\n"
        "}\n";

    sglp_GLuint shader = sglp_load_and_compile_shaders(vert, frag);
    sglp_global_opengl->glUseProgram(shader);
    sglp_global_uniform_sprite_offset = sglp_global_opengl->glGetUniformLocation(shader, "uniform_offset");
    sglp_global_uniform_pos           = sglp_global_opengl->glGetUniformLocation(shader, "sglp_global_uniform_pos");
    sglp_global_uniform_colour        = sglp_global_opengl->glGetUniformLocation(shader, "sglp_global_uniform_colour");
    sglp_global_uniform_no_texture    = sglp_global_opengl->glGetUniformLocation(shader, "uniform_bool_no_texture");
}

static void sglp_generate_2d_mesh(sglp_Sprite *sprite, float no_frames_x, float no_frames_y) {
    int32_t const vert_buf_index = 0;
    int32_t const tex_coor_buf_index = 1;

#define SGLP_VERT_COL 4
#define SGLP_VERT_ROW 3

    float vertices[SGLP_VERT_COL][SGLP_VERT_ROW] = {
        {-1.0f,  1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f},
        {1.0f,   1.0f, 0.0f},
        {1.0f,  -1.0f, 0.0f}
    };

#define SGLP_TEX_COL 4
#define SGLP_TEX_ROW 3

    float tex_height = 1.0f / no_frames_y;
    float tex_width = 1.0f / no_frames_x;
    sglp_GLfloat tex_coords[SGLP_TEX_COL][SGLP_TEX_ROW] = {
        {0.0f,      0.0f,       0.0f},
        {0.0f,      tex_height, 0.0f},
        {tex_width, 0.0f,       0.0f},
        {tex_width, tex_height, 0.0f}
    };

    sglp_global_opengl->glGenVertexArrays(1, &sprite->mesh);
    sglp_global_opengl->glBindVertexArray(sprite->mesh);
    sglp_global_opengl->glGenBuffers(1, &sprite->vbo_arr[vert_buf_index]);

    sglp_global_opengl->glBindBuffer(SGLP_GL_ARRAY_BUFFER, sprite->vbo_arr[vert_buf_index]);
    sglp_global_opengl->glBufferData(SGLP_GL_ARRAY_BUFFER, (SGLP_VERT_COL * SGLP_VERT_ROW * sizeof(vertices[0])), vertices, SGLP_GL_STATIC_DRAW);
    sglp_global_opengl->glVertexAttribPointer(vert_buf_index, 3, SGLP_GL_FLOAT, SGLP_GL_FALSE, 0, 0);
    sglp_global_opengl->glEnableVertexAttribArray(vert_buf_index);

    sglp_global_opengl->glGenBuffers(1, &sprite->vbo_arr[tex_coor_buf_index]);
    sglp_global_opengl->glBindBuffer(SGLP_GL_ARRAY_BUFFER, sprite->vbo_arr[tex_coor_buf_index]);
    sglp_global_opengl->glBufferData(SGLP_GL_ARRAY_BUFFER, (SGLP_TEX_COL * SGLP_TEX_ROW * sizeof(tex_coords[0])), tex_coords, SGLP_GL_STATIC_DRAW);
    sglp_global_opengl->glVertexAttribPointer(tex_coor_buf_index, 3, SGLP_GL_FLOAT, SGLP_GL_FALSE, 0, 0);
    sglp_global_opengl->glEnableVertexAttribArray(tex_coor_buf_index);
}

sglp_Sprite sglp_load_image(sglp_API *api, uint8_t *img_data, int32_t frame_cnt_x, int32_t frame_cnt_y,
                            int32_t id, int32_t width, int32_t height, int32_t no_components) {
    int i, j;
    sglp_Sprite res = {0};
    res.id = id;
    res.w = width;
    res.h = height;
    res.frame_cnt_x = frame_cnt_x;
    res.frame_cnt_y = frame_cnt_y;

    sglp_global_opengl->glBindTexture(SGLP_GL_TEXTURE_2D, id);

    switch(no_components) {
        case 4: {
            sglp_global_opengl->glTexImage2D(SGLP_GL_TEXTURE_2D, 0, SGLP_GL_RGBA, width, height, 0, SGLP_GL_RGBA,
                                             SGLP_GL_UNSIGNED_BYTE, img_data);
        } break;

        case 3: {
            sglp_global_opengl->glTexImage2D(SGLP_GL_TEXTURE_2D, 0, SGLP_GL_RGB, width, height, 0, SGLP_GL_RGB,
                                             SGLP_GL_UNSIGNED_BYTE, img_data);
        } break;

        // TODO(Jonny): Right now, a 1 component bitmap is converted into a 4 component one and loaded in. Is there
        //              a less-stupid way to do this?
        case 1: {
            uint8_t *rgba_bitmap = (char unsigned *)api->os_malloc(width * height * 4);
            uint8_t *src = img_data;

            uint8_t *dst_row = rgba_bitmap;
            for(i = 0; (i < height); ++i) {
                int32_t *dst = (int32_t *)dst_row;
                for(j = 0; (j < width); ++j) {
                    uint8_t alpha = *src++;
                    *dst++ = ((alpha << 24) | (alpha << 16) | (alpha << 8) | (alpha << 0));
                }

                dst_row += width * 4;
            }

            sglp_global_opengl->glTexImage2D(SGLP_GL_TEXTURE_2D, 0, SGLP_GL_RGBA, width, height, 0, SGLP_GL_RGBA,
                                             SGLP_GL_UNSIGNED_BYTE, rgba_bitmap);

            api->os_free(rgba_bitmap);
        } break;

        default: {
            SGLP_ASSERT(0);
            goto load_image_exit;
        }
    }

    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_MIN_FILTER, SGLP_GL_NEAREST);
    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_MAG_FILTER, SGLP_GL_NEAREST);
    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_MIN_LOD, 0);
    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_MAX_LOD, 0);
    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_BASE_LEVEL, 0);
    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_MAX_LEVEL, 0);

    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_WRAP_S, SGLP_GL_CLAMP);
    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_WRAP_T, SGLP_GL_CLAMP);
    sglp_global_opengl->glTexParameteri(SGLP_GL_TEXTURE_2D, SGLP_GL_TEXTURE_WRAP_R, SGLP_GL_CLAMP);

    sglp_global_opengl->glBindTexture(SGLP_GL_TEXTURE_2D, 0);
    sglp_generate_2d_mesh(&res, frame_cnt_x, frame_cnt_y);

load_image_exit:;

    return(res);
}

static sglp_Sprite sglp_global_black_sprite;

static void sglp_pass_mat4x4_to_shader(float *buf, uint32_t uniform) {
    sglp_global_opengl->glUniformMatrix4fv(uniform, 1, SGLP_GL_FALSE, buf);
}

static int32_t sglp_string_len(char const *str) {
    int32_t res = 0;
    while(str[res++]);

    return(res);
}

uint32_t sglp_load_and_compile_shaders(char const *fvertex, char const *ffragment) {
    sglp_GLuint res = 0;

    int32_t vert_compiled = 0;
    int32_t frag_compiled = 0;

    uint32_t vert_shader = sglp_global_opengl->glCreateShader(SGLP_GL_VERTEX_SHADER);
    uint32_t frag_shader = sglp_global_opengl->glCreateShader(SGLP_GL_FRAGMENT_SHADER);

    int32_t frag_len = sglp_string_len(ffragment);
    int32_t vert_len = sglp_string_len(fvertex);

    sglp_global_opengl->glShaderSource(vert_shader, 1, &fvertex, &vert_len);
    sglp_global_opengl->glShaderSource(frag_shader, 1, &ffragment, &frag_len);

    sglp_global_opengl->glCompileShader(vert_shader);
    sglp_global_opengl->glGetShaderiv(vert_shader, SGLP_GL_COMPILE_STATUS, &vert_compiled);

    sglp_global_opengl->glCompileShader(frag_shader);
    sglp_global_opengl->glGetShaderiv(frag_shader, SGLP_GL_COMPILE_STATUS, &frag_compiled);

    if((vert_compiled) && (frag_compiled)) {
        res = sglp_global_opengl->glCreateProgram();

        sglp_global_opengl->glAttachShader(res, vert_shader);
        sglp_global_opengl->glAttachShader(res, frag_shader);

        sglp_global_opengl->glLinkProgram(res);
        sglp_global_opengl->glUseProgram(res);

    } else if((!vert_compiled) && (!frag_compiled)) {
        sglp_print_shader_err(vert_shader);
        sglp_print_shader_err(frag_shader);
    } else if(!vert_compiled) {
        sglp_print_shader_err(vert_shader);
    } else {
        sglp_print_shader_err(frag_shader);
    }

    return(res);
}

void sglp_clear_screen_for_frame() {
    sglp_global_opengl->glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    sglp_global_opengl->glClear(SGLP_GL_COLOR_BUFFER_BIT);

    sglp_global_opengl->glUniform2f(sglp_global_uniform_sprite_offset, 0.0f, 0.0f);
    sglp_global_opengl->glUniform4f(sglp_global_uniform_colour, 1.0f, 1.0f, 1.0f, 1.0f);
}

// TODO - The two sglp_sprite_sprite calls can probably be refactored down to one.
void sglp_draw_sprite(sglp_Sprite sprite, int32_t cur_frame, float const *tform) {
    // Convert the x and y into opengl x and y.

    float my_tform[16] = { tform[0], tform[1], tform[2], tform[3],
                           tform[4], tform[5], tform[6], tform[7],
                           tform[8], tform[9], tform[10], tform[11],
                           sglp_screen_to_gl_x(tform[12]), -sglp_screen_to_gl_y(tform[13]), tform[14], tform[15]
                         };

    int x_increment = cur_frame;
    int y_increment = 0;

    sglp_global_opengl->glBindVertexArray(sprite.mesh);
    sglp_global_opengl->glBindTexture(SGLP_GL_TEXTURE_2D, sprite.id);

    while(x_increment >= sprite.frame_cnt_x) {
        x_increment -= sprite.frame_cnt_x;
        ++y_increment;
    }

    float frame_pos_x = ((1.0f / sprite.frame_cnt_x) * x_increment) * -1.0f;
    float frame_pos_y = ((1.0f / sprite.frame_cnt_y) * y_increment) * -1.0f;

    sglp_global_opengl->glUniform2f(sglp_global_uniform_sprite_offset, frame_pos_x, frame_pos_y);

    sglp_pass_mat4x4_to_shader(my_tform, sglp_global_uniform_pos);
    sglp_global_opengl->glDrawArrays(SGLP_GL_TRIANGLE_STRIP, 0, 4);
}

void sglp_draw_sprite_frame_matrix(sglp_Sprite sprite, int32_t cur_frame_x, int32_t cur_frame_y, float const *tform) {
    float my_tform[16] = { tform[0], tform[1], tform[2], tform[3],
                           tform[4], tform[5], tform[6], tform[7],
                           tform[8], tform[9], tform[10], tform[11],
                           sglp_screen_to_gl_x(tform[12]), -sglp_screen_to_gl_y(tform[13]), tform[14], tform[15]
                         };

    sglp_global_opengl->glBindVertexArray(sprite.mesh);
    sglp_global_opengl->glBindTexture(SGLP_GL_TEXTURE_2D, sprite.id);

    float frame_pos_x = ((1.0f / sprite.frame_cnt_x) * cur_frame_x) * -1.0f;
    float frame_pos_y = ((1.0f / sprite.frame_cnt_y) * cur_frame_y) * -1.0f;

    sglp_global_opengl->glUniform2f(sglp_global_uniform_sprite_offset, frame_pos_x, frame_pos_y);

    sglp_pass_mat4x4_to_shader(my_tform, sglp_global_uniform_pos);
    sglp_global_opengl->glDrawArrays(SGLP_GL_TRIANGLE_STRIP, 0, 4);
}

void sglp_draw_black_box(float const *tform) {
    float my_tform[16] = { tform[0], tform[1], tform[2], tform[3],
                           tform[4], tform[5], tform[6], tform[7],
                           tform[8], tform[9], tform[10], tform[11],
                           sglp_screen_to_gl_x(tform[12]), -sglp_screen_to_gl_y(tform[13]), tform[14], tform[15]
                         };

    sglp_global_opengl->glBindVertexArray(sglp_global_black_sprite.mesh);
    sglp_global_opengl->glBindTexture(SGLP_GL_TEXTURE_2D, sglp_global_black_sprite.id);

    sglp_global_opengl->glUniform2f(sglp_global_uniform_sprite_offset, 0.0f, 0.0f);

    sglp_pass_mat4x4_to_shader(my_tform, sglp_global_uniform_pos);
    sglp_global_opengl->glDrawArrays(SGLP_GL_TRIANGLE_STRIP, 0, 4);
}

//
// Audio
//
typedef struct sglp_LoadedSound {
    uint32_t sample_cnt; // This is the sample count divide by 8
    uint32_t no_channels;
    int16_t *samples[2];
    int32_t id;
} sglp_LoadedSound;

typedef struct sglp_SoundOutputBuffer {
    int16_t *samples; // samples must be a multiple of 8!
    int32_t samples_per_second;
    int32_t sample_cnt;
} sglp_SoundOutputBuffer;

static sglp_LoadedSound *sglp_global_loaded_sounds = 0;
static int32_t sglp_global_loaded_sound_cnt = 0;
static int32_t sglp_global_loaded_sound_max = 0;

typedef struct sglp_AudioState {
    sglp_PlayingSound *first_playing_snd;
    sglp_PlayingSound *first_free_playing_snd;
} sglp_AudioState;
static sglp_AudioState sglp_global_audio_state;

static void sglp_set_max_no_of_sounds(sglp_API *api, int32_t n) {
    int i;
    sglp_global_loaded_sound_max = n;
    sglp_global_loaded_sounds = (sglp_LoadedSound *)api->os_malloc(sizeof(sglp_LoadedSound) * sglp_global_loaded_sound_max);

    for(i = 0; (i < sglp_global_loaded_sound_max); ++i) {
        sglp_global_loaded_sounds[i].id = 0xFFFFFFFF;
    }
}

sglp_PlayingSound *sglp_get_playing_sound(int32_t id) {
    sglp_PlayingSound *res = sglp_global_audio_state.first_playing_snd;
    while(res) {
        if(res->id == id) {
            break;
        }

        res = res->next;
    }

    return(res);
}

sglp_PlayingSound *sglp_play_new_audio(sglp_API *api, int32_t id) {
    sglp_PlayingSound *res = 0;
    if(!sglp_global_audio_state.first_free_playing_snd) {
        sglp_global_audio_state.first_free_playing_snd = (sglp_PlayingSound *)api->os_malloc(sizeof(sglp_PlayingSound));
    }

    res = sglp_global_audio_state.first_free_playing_snd;
    sglp_global_audio_state.first_free_playing_snd = res->next;

    res->samples_played = 0;
    res->cur_volume0 = res->target_volume0 = 1.0f;
    res->cur_volume1 = res->target_volume1 = 1.0f;
    res->dcur_volume0 = 0.0f; res->dcur_volume1 = 0.0f;
    res->id = id;
    res->dsample = 1.0f;

    res->next = sglp_global_audio_state.first_playing_snd;
    sglp_global_audio_state.first_playing_snd = res;

    return(res);
}

sglp_PlayingSound *sglp_play_audio(sglp_API *api, int32_t id) {
    sglp_PlayingSound *res = sglp_get_playing_sound(id);
    if(!res) {
        res = sglp_play_new_audio(api, id);
    }

    return(res);
}

void sglp_change_volume(sglp_PlayingSound *snd, float FadeDurationSeconds, float vol0, float vol1) {
    if(FadeDurationSeconds <= 0.0f) {
        snd->cur_volume0 = snd->target_volume0 = vol0;
        snd->cur_volume1 = snd->target_volume1 = vol1;
    } else {
        snd->target_volume0 = vol0;
        snd->target_volume1 = vol1;

        snd->dcur_volume0 = snd->target_volume0 - snd->cur_volume0;
        snd->dcur_volume1 = snd->target_volume1 - snd->cur_volume1;

        snd->dcur_volume0 = snd->dcur_volume0 / FadeDurationSeconds;
        snd->dcur_volume1 = snd->dcur_volume1 / FadeDurationSeconds;
    }
}

void sglp_change_pitch(sglp_PlayingSound *snd, float dsample) {
    snd->dsample = dsample;
}

static sglp_LoadedSound *get_sound(sglp_LoadedSound *Sounds, int32_t id) {
    int i;
    sglp_LoadedSound *res = 0;

    for(i = 0; (i < sglp_global_loaded_sound_cnt); ++i) {
        if(Sounds[i].id == id) {
            res = &Sounds[i];
            break;
        }
    }

    return(res);
}

static int32_t sglp_round_float_to_int(float num) {
    int32_t res = (int32_t)(num + 0.5f);

    return(res);
}

static int32_t sglp_floor_float_to_int(float num) {
    int32_t res = (int32_t)num;

    return(res);
}

static float sglp_lerp(float t, float a, float b) {
    float res = (1 - t) * a + t * b;

    return(res);
}

static void sglp_output_playing_sounds(sglp_API *api, sglp_SoundOutputBuffer *snd_buf) {
    int i;
    float master_vol0 = 1.0f, master_vol1 = 1.0f;
    float seconds_per_sample = 1.0f / (float)snd_buf->samples_per_second;

    // TODO(Jonny): Could I get rid of this memory allocation and use a  buffer?
    float *float_channel0 = (float *)api->os_malloc(sizeof(float *) * snd_buf->sample_cnt);
    float *float_channel1 = (float *)api->os_malloc(sizeof(float *) * snd_buf->sample_cnt);

    SGLP_ASSERT((snd_buf->sample_cnt & 3) == 0);

    // Clear out the mixer channels.
    {
        float *dst0 = (float *)float_channel0;
        float *dst1 = (float *)float_channel1;

        for(i = 0; (i < snd_buf->sample_cnt); ++i) {
            *dst0++ = 0.0f;
            *dst1++ = 0.0f;
        }
    }

    // Sum all the sounds.
    {
        sglp_PlayingSound **playing_snd_ptr;
        for(playing_snd_ptr = &sglp_global_audio_state.first_playing_snd; (*playing_snd_ptr); /* Empty */) {
            sglp_PlayingSound *playing_snd = *playing_snd_ptr;
            sglp_Bool snd_finished = SGLP_FALSE;

            uint32_t total_samples_to_mix = snd_buf->sample_cnt;
            float *dst0 = (float *)float_channel0;
            float *dst1 = (float *)float_channel1;

            while((total_samples_to_mix) && (!snd_finished)) {
                sglp_LoadedSound *loaded_snd = get_sound(sglp_global_loaded_sounds, playing_snd->id);
                float dvol0 = playing_snd->dcur_volume0 * seconds_per_sample;
                float dvol1 = playing_snd->dcur_volume1 * seconds_per_sample;
                uint32_t samples_to_mix, samples_remaining_in_sound;
                sglp_Bool vol_ended[2] = {0};
                float sample_pos;

                SGLP_ASSERT(loaded_snd);
                SGLP_ASSERT(playing_snd->samples_played >= 0.0f); // Do now allow pre-queued sounds (right now).

                samples_to_mix = total_samples_to_mix;
                samples_remaining_in_sound = sglp_round_float_to_int((loaded_snd->sample_cnt - sglp_round_float_to_int(playing_snd->samples_played)) /
                                                                     playing_snd->dsample);
                if(samples_to_mix > samples_remaining_in_sound) {
                    samples_to_mix = samples_remaining_in_sound;
                }

                // Channel 0.
                {
                    if(dvol0 != 0.0f) {
                        uint32_t vol_sample_cnt = (uint32_t)(((playing_snd->target_volume0 - playing_snd->cur_volume0) /
                                                              dvol0) + 0.5f);
                        if(samples_to_mix > vol_sample_cnt) {
                            samples_to_mix = vol_sample_cnt;
                            vol_ended[0] = SGLP_TRUE;
                        }
                    }
                }

                // Channel 1.
                {
                    if(dvol1 != 0.0f) {
                        uint32_t vol_sample_cnt = (uint32_t)(((playing_snd->target_volume1 - playing_snd->cur_volume1) /
                                                              dvol1) + 0.5f);
                        if(samples_to_mix > vol_sample_cnt) {
                            samples_to_mix = vol_sample_cnt;
                            vol_ended[1] = SGLP_TRUE;
                        }
                    }
                }

                // TODO(Jonny): Handle stereo!
                sample_pos = playing_snd->samples_played;
                for(i = 0; (i < samples_to_mix); ++i) {
                    uint32_t sample_index = sglp_floor_float_to_int(sample_pos);
                    float sample_value = sglp_lerp(sample_pos - (float)sample_index,
                                                   (float)loaded_snd->samples[0][sample_index],
                                                   (float)loaded_snd->samples[0][sample_index + 1]);

                    *dst0++ += master_vol0 * playing_snd->cur_volume0 * sample_value;
                    *dst1++ += master_vol1 * playing_snd->cur_volume1 * sample_value;

                    playing_snd->cur_volume0 = playing_snd->cur_volume0 + dvol0;
                    playing_snd->cur_volume1 = playing_snd->cur_volume1 + dvol1;
                    sample_pos += playing_snd->dsample;
                }

                if(vol_ended[0]) {
                    playing_snd->cur_volume0 = playing_snd->target_volume0;
                    playing_snd->dcur_volume0 = 0.0f;
                }

                if(vol_ended[1]) {
                    playing_snd->cur_volume1 = playing_snd->target_volume1;
                    playing_snd->dcur_volume1 = 0.0f;
                }

                SGLP_ASSERT(total_samples_to_mix >= samples_to_mix);
                playing_snd->samples_played = sample_pos;
                total_samples_to_mix -= samples_to_mix;

                snd_finished = (playing_snd->samples_played == loaded_snd->sample_cnt);
            }

            if (snd_finished) {
                *playing_snd_ptr = playing_snd->next;
                playing_snd->next = sglp_global_audio_state.first_free_playing_snd;
                sglp_global_audio_state.first_free_playing_snd = playing_snd;
            } else {
                playing_snd_ptr = &playing_snd->next;
            }
        }
    }

    // Convert the audio sample to 16-bits.
    {
        float *src0 = (float *)float_channel0;
        float *src1 = (float *)float_channel1;

        int16_t *sample_out = snd_buf->samples;
        for(i = 0; (i < snd_buf->sample_cnt); ++i) {
            *sample_out++ = (int16_t)(*src0++ + 0.5f);
            *sample_out++ = (int16_t)(*src1++ + 0.5f);
        }
    }

    api->os_free(float_channel1);
    api->os_free(float_channel0);
}

//
// Wav Loader
//

#pragma pack(push, 1)
typedef struct sglp_WAVEHeader {
    uint32_t riff_id;
    uint32_t size;
    uint32_t wav_id;
} sglp_WAVEHeader;

#define SGLP_RIFF_CODE(a, b, c, d) (((uint32_t)(a) << 0) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
int const WAVE_ChunkID_fmt  = SGLP_RIFF_CODE('f', 'm', 't', ' ');
int const WAVE_ChunkID_data = SGLP_RIFF_CODE('d', 'a', 't', 'a');
int const WAVE_ChunkID_RIFF = SGLP_RIFF_CODE('R', 'I', 'F', 'F');
int const WAVE_ChunkID_WAVE = SGLP_RIFF_CODE('W', 'A', 'V', 'E');
#undef SGLP_RIFF_CODE

typedef struct sglp_WavChunk {
    uint32_t id;
    uint32_t size;
} sglp_WavChunk;

typedef struct sglp_WavFormat {
    int16_t wFormatTag;
    int16_t nChannels;
    uint32_t  nSamplesPerSec;
    uint32_t  nAvgBytesPerSec;
    int16_t nBlockAlign;
    int16_t wBitsPerSample;
    int16_t cbSize;
    int16_t wValidBitsPerSample;
    uint32_t  dwChannelMask;
    uint8_t SubFormat[16];
} sglp_WavFormat;
#pragma pack(pop)

typedef struct sglp_RiffIter {
    uint8_t *at;
    uint8_t *stop;
} sglp_RiffIter;

static sglp_RiffIter sglp_parse_chunk_at(void *at, void *stop) {
    sglp_RiffIter iter;
    iter.at = (uint8_t *)at;
    iter.stop = (uint8_t *)stop;

    return(iter);
}

static sglp_RiffIter sglp_next_chunk(sglp_RiffIter iter) {
    sglp_WavChunk *chunk = (sglp_WavChunk *)iter.at;
    uint32_t size = (chunk->size + 1) & ~1;
    iter.at += sizeof(sglp_WavChunk) + size;

    return(iter);
}

static sglp_Bool sglp_is_valid(sglp_RiffIter iter) {
    sglp_Bool res = (iter.at < iter.stop);

    return(res);
}

static void *sglp_get_chunk_data(sglp_RiffIter iter) {
    void *res = iter.at + sizeof(sglp_WavChunk);

    return(res);
}

static uint32_t sglp_get_type(sglp_RiffIter iter) {
    sglp_WavChunk *chunk = (sglp_WavChunk *)iter.at;
    uint32_t res = chunk->id;

    return(res);
}

static uint32_t sglp_get_chunk_data_size(sglp_RiffIter iter) {
    sglp_WavChunk *chunk = (sglp_WavChunk *)iter.at;
    uint32_t res = chunk->size;

    return(res);
}

sglp_Bool sglp_load_wav(sglp_API *api, int32_t id, void *data, uintptr_t size) {
    sglp_Bool res = SGLP_FALSE;
    if(sglp_global_loaded_sound_cnt + 1< sglp_global_loaded_sound_max) {
        int i, j;
        sglp_WAVEHeader *header;
        sglp_LoadedSound *loaded_snd = sglp_global_loaded_sounds + sglp_global_loaded_sound_cnt++;
        uint32_t no_channels = 0, sample_data_size = 0, sample_cnt;
        int16_t *sample_data = 0;
        sglp_RiffIter iter;

        loaded_snd->id = id;

        header = (sglp_WAVEHeader *)api->os_malloc(size);
        sglp_memcpy(header, data, size);
        SGLP_ASSERT((header) && (header->riff_id == WAVE_ChunkID_RIFF) && (header->wav_id == WAVE_ChunkID_WAVE));

        for(iter = sglp_parse_chunk_at((header + 1), ((uint8_t *)(header + 1) + header->size - 4)); (sglp_is_valid(iter)); iter = sglp_next_chunk(iter)) {
            switch(sglp_get_type(iter)) {
                case WAVE_ChunkID_fmt: {
                    sglp_WavFormat *fmt = (sglp_WavFormat *)sglp_get_chunk_data(iter);
                    SGLP_ASSERT((fmt->wFormatTag == 1) && (fmt->nSamplesPerSec == 48000) && (fmt->wBitsPerSample == 16) && (fmt->nBlockAlign == sizeof(int16_t) * fmt->nChannels));
                    no_channels = fmt->nChannels;
                } break;

                case WAVE_ChunkID_data: {
                    sample_data = (int16_t *)sglp_get_chunk_data(iter);
                    sample_data_size = sglp_get_chunk_data_size(iter);
                } break;
            }
        }

        SGLP_ASSERT((no_channels) && (sample_data));

        loaded_snd->no_channels = no_channels;
        sample_cnt = sample_data_size / (no_channels * sizeof(int16_t));
        if(no_channels == 1) {
            loaded_snd->samples[0] = (int16_t *)sample_data;
            loaded_snd->samples[1] = 0;
        } else if(no_channels == 2) {
            loaded_snd->samples[0] = sample_data;
            loaded_snd->samples[1] = sample_data + sample_cnt;

            for(i = 0; (i < sample_cnt); ++i) {
                int16_t Source = sample_data[2 * i];
                sample_data[2 * i] = sample_data[i];
                sample_data[i]   = Source;
            }
        } else {
            res = SGLP_FALSE;
            SGLP_ASSERT(SGLP_FALSE);
        }

        // TODO(Jonny): Load right channel!
        loaded_snd->no_channels = 1;

        for(i = loaded_snd->no_channels; (i < loaded_snd->no_channels); ++i) {
            for(j = sample_cnt; (j < sample_cnt + 8); ++j) {
                loaded_snd->samples[i][j] = 0;
            }
        }

        loaded_snd->sample_cnt = sample_cnt;
        res = SGLP_TRUE;
    }

    return(res);
}

static void sglp_setup(sglp_API *api, int32_t max_no_of_sounds) {
    uint8_t img_data[] = {0, 0, 0, 0xFF};
    sglp_global_black_sprite = sglp_load_image(api, img_data, 1, 1, 0xFFFF/*Do properly*/, 1, 1, 4);

    sglp_setup_default_shader();
    sglp_set_max_no_of_sounds(api, max_no_of_sounds);
}

static void sglp_free_file(sglp_API *api, sglp_File *file) {
    if(file->e) {
        api->os_free(file->e);
    }
}

//
// SDL
//
#if defined(SGLP_USE_SDL)

// TODO - Let the user include this?
#include <SDL2/SDL.h>

static sglp_File sglp_sdl_read_file(sglp_API *api, char const *fname) {
    sglp_File res = {0};

    FILE *file = fopen(fname, "rb");
    if(file) {
        fseek(file, 0, SEEK_END);
        res.size = ftell(file);
        fseek(file, 0, SEEK_SET);

        res.e = (uint8_t *)api->os_malloc(res.size);
        fread(res.e, res.size, 1, file);
        fclose(file);
    }

    return(res);
}

static sglp_File sglp_sdl_read_file_and_null_terminate(sglp_API *api, char const *fname) {
    sglp_File res = {0};

    FILE *file = fopen(fname, "rb");
    if(file) {
        fseek(file, 0, SEEK_END);
        res.size = ftell(file);
        fseek(file, 0, SEEK_SET);

        res.e = (uint8_t *)api->os_malloc(res.size + 1);
        fread(res.e, res.size, 1, file);
        fclose(file);
    }

    return(res);
}

static uint64_t sglp_sdl_get_processor_timestamp(void) {
    return(0); // TODO - Implement.
}

//
// Threading stuff
//
typedef struct sglp_SdlWorkQueueEntry {
    void (*callback)(void *data);
    void *e;
} sglp_SdlWorkQueueEntry;

typedef struct sglp_SdlWorkQueue {
    int volatile goal;
    int volatile cnt;

    int volatile next_entry_to_write;
    int volatile next_entry_to_read;

    SDL_sem *hsem;
    sglp_SdlWorkQueueEntry entries[256]; // TODO(Jonny): Should I make this a Linked List?? Or a dynamically resizing array??
} sglp_SdlWorkQueue;
static sglp_SdlWorkQueue sglp_global_work_queue;

static void sglp_sdl_add_work_queue_entry(void *data, void (*callback)(void *data)) {
    int new_next_entry_to_write = (sglp_global_work_queue.next_entry_to_write + 1) % SGLP_ARRAY_COUNT(sglp_global_work_queue.entries);
    sglp_SdlWorkQueueEntry *entry = sglp_global_work_queue.entries + sglp_global_work_queue.next_entry_to_write;

    SGLP_ASSERT(new_next_entry_to_write != sglp_global_work_queue.next_entry_to_read);

    entry->callback = callback;
    entry->e = data;

    ++sglp_global_work_queue.goal;

    // TODO(Jonny): Put a full compiler/write barrier here!

    sglp_global_work_queue.next_entry_to_write = new_next_entry_to_write;
    SDL_DestroySemaphore(sglp_global_work_queue.hsem);
    //ReleaseSemaphore(sglp_global_work_queue.hsem, 1, 0);
    //if(sglp_pthread_sem_close) {
    //    sglp_pthread_sem_close(sglp_global_work_queue.hsem);
    //}
}

static sglp_Bool sglp_sdl_do_next_work_queue_entry(sglp_SdlWorkQueue *work_queue) {
    sglp_Bool res = SGLP_FALSE;

    int original_next_entry_to_read = work_queue->next_entry_to_read;
    int new_next_entry_to_read = (original_next_entry_to_read + 1) % SGLP_ARRAY_COUNT(work_queue->entries);
    if(original_next_entry_to_read != work_queue->next_entry_to_write) {
        int i = __sync_val_compare_and_swap(&work_queue->next_entry_to_read, original_next_entry_to_read, new_next_entry_to_read);
        if(i == original_next_entry_to_read) {
            sglp_SdlWorkQueueEntry entry = work_queue->entries[i];
            entry.callback(entry.e);
            SDL_atomic_t x = {work_queue->cnt};
            //__sync_fetch_and_add(&work_queue->cnt, 1);
            SDL_AtomicIncRef(&x);
            work_queue->cnt = x.value;
        }
    } else {
        res = SGLP_TRUE;
    }

    return(res);
}

static void sglp_sdl_complete_all_work(void) {
    while(sglp_global_work_queue.goal != sglp_global_work_queue.cnt) {
        sglp_sdl_do_next_work_queue_entry(&sglp_global_work_queue);
    }

    sglp_global_work_queue.goal = 0;
    sglp_global_work_queue.cnt = 0;
}

static int sglp_sdl_thread_proc(void *Parameter) {
    sglp_SdlWorkQueue *queue = (sglp_SdlWorkQueue *)Parameter;
    for(;;) {
        if(sglp_sdl_do_next_work_queue_entry(queue)) {
            // TODO(Jonny): Find a function on Linux that will wait for the thread.
        }
    }

    return(0);
}

//
// Memory
//
static void *sglp_sdl_malloc(uintptr_t size) {
    void *res = malloc(size);
    if(res) {
        sglp_memset(res, 0, size);
    }

    return(res);
}

static void *sglp_sdl_realloc(void *ptr, uintptr_t size) {
    void *res = realloc(ptr, size);

    return(res);
}

static void sglp_sdl_free(void *ptr) {
    if(ptr) {
        free(ptr);
    }
}

//
// OpenGL
//
static SDL_GLContext sglp_sdl_init_opengl(SDL_Window *window) {
    SDL_GLContext res = SDL_GL_CreateContext(window);
    if(res) {
        int context_major_version;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &context_major_version);
        sglp_Bool modern_context = context_major_version >= 3;

#define SGLP_SDL_OPENGL_LOAD_(gl_func, gl_func_t, gl_func_str) sglp_global_opengl->gl_func = (sglp_##gl_func_t *)SDL_GL_GetProcAddress(gl_func_str); if(all_gl_funcs_loaded) { all_gl_funcs_loaded = (sglp_global_opengl->gl_func) != 0; } else { SGLP_ASSERT(0); }
#define SGLP_SDL_OPENGL_LOAD(gl_func) do { SGLP_SDL_OPENGL_LOAD_(gl_func, gl_func##_t, #gl_func) } while(0)

        sglp_Bool all_gl_funcs_loaded = SGLP_TRUE;

        SGLP_SDL_OPENGL_LOAD(glBindTexture);
        SGLP_SDL_OPENGL_LOAD(glClear);
        SGLP_SDL_OPENGL_LOAD(glClearColor);
        SGLP_SDL_OPENGL_LOAD(glDrawArrays);
        SGLP_SDL_OPENGL_LOAD(glGetError);
        SGLP_SDL_OPENGL_LOAD(glTexImage2D);
        SGLP_SDL_OPENGL_LOAD(glTexParameteri);
        SGLP_SDL_OPENGL_LOAD(glGetString);
        SGLP_SDL_OPENGL_LOAD(glViewport);

        SGLP_SDL_OPENGL_LOAD(glCreateShader);
        SGLP_SDL_OPENGL_LOAD(glShaderSource);
        SGLP_SDL_OPENGL_LOAD(glCompileShader);
        SGLP_SDL_OPENGL_LOAD(glGetShaderiv);
        SGLP_SDL_OPENGL_LOAD(glCreateProgram);
        SGLP_SDL_OPENGL_LOAD(glAttachShader);
        SGLP_SDL_OPENGL_LOAD(glBindAttribLocation);
        SGLP_SDL_OPENGL_LOAD(glLinkProgram);
        SGLP_SDL_OPENGL_LOAD(glUseProgram);
        SGLP_SDL_OPENGL_LOAD(glGenBuffers);
        SGLP_SDL_OPENGL_LOAD(glBindBuffer);
        SGLP_SDL_OPENGL_LOAD(glBufferData);
        SGLP_SDL_OPENGL_LOAD(glVertexAttribPointer);
        SGLP_SDL_OPENGL_LOAD(glEnableVertexAttribArray);
        SGLP_SDL_OPENGL_LOAD(glUniform1i);
        SGLP_SDL_OPENGL_LOAD(glUniform2f);
        SGLP_SDL_OPENGL_LOAD(glUniform4f);
        SGLP_SDL_OPENGL_LOAD(glUniformMatrix4fv);
        SGLP_SDL_OPENGL_LOAD(glGetUniformLocation);
        SGLP_SDL_OPENGL_LOAD(glIsShader);
        SGLP_SDL_OPENGL_LOAD(glGetProgramiv);
        SGLP_SDL_OPENGL_LOAD(glGetProgramInfoLog);
        SGLP_SDL_OPENGL_LOAD(glGetShaderInfoLog);

        SGLP_SDL_OPENGL_LOAD(glGenVertexArrays);
        SGLP_SDL_OPENGL_LOAD(glBindVertexArray);

#undef SGLP_SDL_OPENGL_LOAD


        SDL_GL_SetSwapInterval(1);
    }

    return(res);
}

static SDL_Window *sglp_sdl_create_dummy_window(void) {
    return SDL_CreateWindow("Temp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_HIDDEN|SDL_WINDOW_OPENGL);
}

static void sglp_sdl_set_opengl_attributes(void) {
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    int context_flag = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#if defined(SGLP_INTERNAL)
    context_flag |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flag);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
#if 0
    SDL_Window *dummy_window = sglp_sdl_create_dummy_window();
    if(!dummy_window) {
        // NOTE: CreateWindow failed. Try again without asking for a SRGB capable framebuffer
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0);

        dummy_window = sglp_sdl_create_dummy_window();
    }

    if(dummy_window) {
        SDL_GLContext context = SDL_GL_CreateContext(dummy_window);
        if(context) {
            // NOTE: CreateContext succeeded. Good.
            SDL_GL_DeleteContext(context);
        } else {
            // NOTE: CreateContext failed. Try setting OpenGL to a lower version
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        }

        SDL_DestroyWindow(dummy_window);
    }
#endif
}

static sglp_Key sglp_sdl_key_to_sgl_key(SDL_Keycode k) {
    sglp_Key res = sglp_key_unknown;
    if(k >= SDLK_a && k <= SDLK_z) {
        res = (sglp_Key)k + ('A' - 'a');
    } else {
        switch(k) {
            case SDLK_RCTRL:  case SDLK_LCTRL:  { res = sglp_key_ctrl;   } break;
            case SDLK_RSHIFT: case SDLK_LSHIFT: { res = sglp_key_shift;  } break;
            case SDLK_SPACE:                    { res = sglp_key_space;  } break;
            case SDLK_ESCAPE:                   { res = sglp_key_escape; } break;
            case SDLK_LEFT:                     { res = sglp_key_left;   } break;
            case SDLK_RIGHT:                    { res = sglp_key_right;  } break;
            case SDLK_UP:                       { res = sglp_key_up;     } break;
            case SDLK_DOWN:                     { res = sglp_key_down;   } break;
        }
    }

    return(res);
}

static void sglp_sdl_process_pending_events(sglp_API *api) {
    SDL_Event event = {0};
    for(;;) {
        int pending_events = SDL_PollEvent(&event);

        if(!pending_events) {
            break;
        }

        switch(event.type) {
            case SDL_QUIT: {
                api->quit = SGLP_TRUE;
            } break;

            case SDL_KEYDOWN: {
                SDL_Keycode key_code = event.key.keysym.sym;
                api->key[sglp_sdl_key_to_sgl_key(key_code)] = 1.0f;
            } break;

            case SDL_KEYUP: {
                SDL_Keycode key_code = event.key.keysym.sym;
                api->key[sglp_sdl_key_to_sgl_key(key_code)] = 0.0f;
            } break;


        }
    }
}

static void sglp_sdl_get_api(sglp_API *api) {
    api->free_file = sglp_free_file;
    api->read_file = sglp_sdl_read_file;
    api->read_file_and_null_terminate = sglp_sdl_read_file_and_null_terminate;

    api->get_processor_timestamp = sglp_sdl_get_processor_timestamp;

    api->add_work_queue_entry = sglp_sdl_add_work_queue_entry;
    api->complete_all_work = sglp_sdl_complete_all_work;

    api->os_malloc = sglp_sdl_malloc;
    api->os_realloc = sglp_sdl_realloc;
    api->os_free = sglp_sdl_free;

    sglp_global_opengl = &api->gl;

    api->settings.frame_rate = 60;
    api->settings.thread_cnt = 8;

    api->settings.win_width = 1920 / 2;
    api->settings.win_height = 1080 / 2;

    api->init_game = SGLP_TRUE;
    api->dt = 16.6f; // TODO - Should be calculated.
}

static uint64_t sglp_sdl_global_perf_count_frequency;

static float sglp_sdl_get_seconds_elapsed(uint64_t start, uint64_t end) {
    return ((float)(end - start) / (float)sglp_sdl_global_perf_count_frequency);
}

static void sglp_sdl_handle_frame_rate_stuff(uint64_t *last_counter, uint64_t *flip_wall_clock,
                                             float *target_seconds_per_frame, float *ms_per_frame) {
    float seconds_elapsed_for_frame = sglp_sdl_get_seconds_elapsed(*last_counter,
                                                                   SDL_GetPerformanceCounter());
    if(seconds_elapsed_for_frame < *target_seconds_per_frame) {
        float test_seconds_elapsed_for_frame;
        uint32_t sleepms = (uint32_t)(1000.0f * (*target_seconds_per_frame - seconds_elapsed_for_frame));
        if(sleepms > 0) {
            SDL_Delay(sleepms);
        }

        test_seconds_elapsed_for_frame = sglp_sdl_get_seconds_elapsed(*last_counter,
                                                                      SDL_GetPerformanceCounter());
        if(test_seconds_elapsed_for_frame < *target_seconds_per_frame) {
            // TODO(Jonny): Log missed Sleep here
        }

        while(seconds_elapsed_for_frame < *target_seconds_per_frame) {
            seconds_elapsed_for_frame = sglp_sdl_get_seconds_elapsed(*last_counter,
                                                                     SDL_GetPerformanceCounter());
        }
    } else {
        // Missed Frame Rate!
    }

    {
        uint64_t end_counter = SDL_GetPerformanceCounter();
        *ms_per_frame = 1000.0f * sglp_sdl_get_seconds_elapsed(*last_counter, end_counter);
        *last_counter = end_counter;
    }

    *flip_wall_clock = SDL_GetPerformanceCounter();
}

typedef struct sglp_SdlSoundOutput {
    int samples_per_second;;
    uint32_t running_sample_index;
    int bytes_per_sample;
    uint32_t secondary_buffer_size;
    uint32_t safety_bytes;
} sglp_SdlSoundOutput;

typedef struct sglp_SdlAudioRingBuffer {
    int size;
    int write_cursor;
    int play_cursor;
    void *data;
} sglp_SdlAudioRingBuffer;
static sglp_SdlAudioRingBuffer sglp_sdl_global_secondary_buffer;

static void sglp_sdl_audio_callback(void *user_data, uint8_t *audio_data, int length) {
    sglp_SdlAudioRingBuffer *ring_buffer = (sglp_SdlAudioRingBuffer *)user_data;

    int region1_size = length;
    int region2_size = 0;
    if((ring_buffer->play_cursor + length) > ring_buffer->size) {
        region1_size = ring_buffer->size - ring_buffer->play_cursor;
        region2_size = length - region1_size;
    }

    sglp_memcpy(audio_data, (uint8_t *)ring_buffer->data + ring_buffer->play_cursor, region1_size);
    sglp_memcpy(audio_data + region1_size, ring_buffer->data, region2_size);
    ring_buffer->play_cursor = (ring_buffer->play_cursor + length) % ring_buffer->size;
    ring_buffer->write_cursor = (ring_buffer->play_cursor + length) % ring_buffer->size;
}

static void sglp_sdl_init_audio(int samples_per_second, int buffer_size) {
    SDL_AudioSpec audio_settings = {0};

    audio_settings.freq = samples_per_second;
    audio_settings.format = AUDIO_S16LSB;
    audio_settings.channels = 2;
    audio_settings.samples = 512;
    audio_settings.callback = &sglp_sdl_audio_callback;
    audio_settings.userdata = &sglp_sdl_global_secondary_buffer;

    sglp_sdl_global_secondary_buffer.size = buffer_size;
    sglp_sdl_global_secondary_buffer.data = sglp_sdl_malloc(buffer_size);

    SDL_OpenAudio(&audio_settings, 0);

    if(audio_settings.format != AUDIO_S16LSB) {
        SGLP_ASSERT(0);
    }
}

static void sglp_sdl_fill_sound_buffer(sglp_SdlSoundOutput *sound_output, int byte_to_lock, int bytes_to_write,
                                       sglp_SoundOutputBuffer *source_buffer) {
    void *region1 = (uint8_t *)sglp_sdl_global_secondary_buffer.data + byte_to_lock;
    int region1_size = bytes_to_write;
    if(region1_size + byte_to_lock > sound_output->secondary_buffer_size) {
        region1_size = sound_output->secondary_buffer_size - byte_to_lock;
    }

    void *region2 = sglp_sdl_global_secondary_buffer.data;
    int region2_size = bytes_to_write - region1_size;

    int region1_sample_count = region1_size / sound_output->bytes_per_sample;
    int16_t *dest_sample = (int16_t *)region1;
    int16_t *source_sample = source_buffer->samples;
    for(int i = 0; (i < region1_sample_count); ++i) {
        *dest_sample++ = *source_sample++;
        *dest_sample++ = *source_sample++;
        ++sound_output->running_sample_index;
    }

    int region2_sample_count = region2_size / sound_output->bytes_per_sample;
    dest_sample = (int16_t *)region2;
    for(int i = 0; (i < region2_sample_count); ++i) {
        *dest_sample++ = *source_sample++;
        *dest_sample++ = *source_sample++;
        ++sound_output->running_sample_index;
    }
}

static void sglp_sdl_handle_audio(sglp_API *api, sglp_SdlSoundOutput *sound_output, int16_t *samples) {
    SDL_LockAudio();
    int byte_to_lock = (sound_output->running_sample_index * sound_output->bytes_per_sample) % sound_output->secondary_buffer_size;
    int target_cursor = ((sglp_sdl_global_secondary_buffer.play_cursor + (sound_output->safety_bytes * sound_output->bytes_per_sample)) % sound_output->secondary_buffer_size);

    int bytes_to_write;
    if(byte_to_lock > target_cursor) {
        bytes_to_write = sound_output->secondary_buffer_size - byte_to_lock;
        bytes_to_write += target_cursor;
    } else {
        bytes_to_write = target_cursor - byte_to_lock;
    }
    SDL_UnlockAudio();

    sglp_SoundOutputBuffer sound_buffer = {0};
    sound_buffer.samples_per_second = sound_output->samples_per_second;
    sound_buffer.sample_cnt = SGLP_ALIGN8(bytes_to_write / sound_output->bytes_per_sample);
    bytes_to_write = sound_buffer.sample_cnt * sound_output->bytes_per_sample;
    sound_buffer.samples = samples;
    sglp_output_playing_sounds(api, &sound_buffer);

    sglp_sdl_fill_sound_buffer(sound_output, byte_to_lock, bytes_to_write, &sound_buffer);
}

static sglp_SdlSoundOutput sglp_sdl_init_sound_output(float game_update_hz) {
    sglp_SdlSoundOutput sound_output = {0};
    sound_output.samples_per_second = 48000;
    sound_output.bytes_per_sample = sizeof(int16_t) * 2;
    sound_output.secondary_buffer_size = sound_output.samples_per_second * sound_output.bytes_per_sample;
    sound_output.safety_bytes = (int)(((float)sound_output.samples_per_second * (float)sound_output.bytes_per_sample / game_update_hz) / 2);

    return(sound_output);
}

int main(int argc, char **argv) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO) == 0) {
        //sglp_sdl_set_opengl_attributes();

        sglp_API api = {0};
        sglp_sdl_get_api(&api);
        sglp_platform_setup_settings_callback(&api.settings);

        sglp_sdl_global_perf_count_frequency = SDL_GetPerformanceFrequency();

        SDL_Window *window = SDL_CreateWindow(api.settings.window_title,
                                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              api.settings.win_width, api.settings.win_height,
                                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if(window) {
            SDL_GLContext opengl_context = sglp_sdl_init_opengl(window);
            if(opengl_context) {
                api.permanent_memory = api.os_malloc(api.settings.permanent_memory_size);
                if(api.permanent_memory) {
                    sglp_SdlWorkQueue work_queue = {0};
                    if(api.settings.thread_cnt > 0) {
                        work_queue.hsem = SDL_CreateSemaphore(0);

                        for(int i = 0; (i < api.settings.thread_cnt); ++i) {
                            SDL_Thread *thread = SDL_CreateThread(sglp_sdl_thread_proc, "Some name.", (void *)&work_queue);
                        }
                    }

                    sglp_setup(&api, api.settings.max_no_of_sounds);

                    int monitor_refresh_hz = 60; // TODO - Allow user to set this.
                    int display_index = SDL_GetWindowDisplayIndex(window);
                    SDL_DisplayMode mode = {0};
                    int display_mode_result = SDL_GetDesktopDisplayMode(display_index, &mode);
                    if(display_mode_result == 0 && mode.refresh_rate > 1) {
                        monitor_refresh_hz = mode.refresh_rate;
                    }
                    float game_update_hz = (float)monitor_refresh_hz;

                    uint64_t last_counter = SDL_GetPerformanceCounter();
                    uint64_t flip_wall_clock = SDL_GetPerformanceCounter();
                    float target_seconds_per_frame = 1.0f / (float)game_update_hz;
                    float ms_per_frame = 0.0f;

                    sglp_SdlSoundOutput sound_output = sglp_sdl_init_sound_output(game_update_hz);
                    sglp_sdl_init_audio(sound_output.samples_per_second, sound_output.secondary_buffer_size);
                    SDL_PauseAudio(0);

                    int max_possible_overrun = 8;
                    int16_t *samples = (int16_t *)sglp_sdl_malloc((sound_output.samples_per_second + max_possible_overrun) * sound_output.bytes_per_sample);
                    SGLP_ASSERT(samples);

                    SDL_ShowWindow(window);
                    sglp_Bool quit = SGLP_FALSE;
                    while(!quit) {
                        api.dt = ms_per_frame;

                        sglp_sdl_process_pending_events(&api);

                        sglp_platform_update_and_render_callback(&api);
                        api.init_game = SGLP_FALSE;
                        SDL_GL_SwapWindow(window);

                        sglp_sdl_handle_audio(&api, &sound_output, samples);

                        sglp_sdl_handle_frame_rate_stuff(&last_counter, &flip_wall_clock,
                                                         &target_seconds_per_frame, &ms_per_frame);
                    }
                }
            }
        }
    } else {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }

    return(0);
}

#else
//
// Win32.
//

#if SGLP_OS_WIN32

// TODO(Jonny): Remove these.
#include <windows.h>
#include <dsound.h>
#include <intrin.h>

// intrin.h
#if defined(__cplusplus)
    extern "C"
#endif
void _WriteBarrier(void);

// xinput.h
typedef struct SGLP_XINPUT_GAMEPAD {
    uint16_t wButtons;
    uint8_t bLeftTrigger, bRightTrigger;
    int16_t sThumbLX, sThumbLY, sThumbRX, sThumbRY;
} SGLP_XINPUT_GAMEPAD;
typedef struct SGLP_XINPUT_STATE {
    uint32_t dwPacketNumber;
    SGLP_XINPUT_GAMEPAD Gamepad;
} SGLP_XINPUT_STATE;
typedef struct SGLP_XINPUT_VIBRATION {
    uint16_t wLeftMotorSpeed, wRightMotorSpeed;
} SGLP_XINPUT_VIBRATION;
#define SGLP_XINPUT_GAMEPAD_DPAD_UP        0x0001
#define SGLP_XINPUT_GAMEPAD_DPAD_DOWN      0x0002
#define SGLP_XINPUT_GAMEPAD_DPAD_LEFT      0x0004
#define SGLP_XINPUT_GAMEPAD_DPAD_RIGHT     0x0008
#define SGLP_XINPUT_GAMEPAD_START          0x0010
#define SGLP_XINPUT_GAMEPAD_BACK           0x0020
#define SGLP_XINPUT_GAMEPAD_LEFT_THUMB     0x0040
#define SGLP_XINPUT_GAMEPAD_RIGHT_THUMB    0x0080
#define SGLP_XINPUT_GAMEPAD_LEFT_SHOULDER  0x0100
#define SGLP_XINPUT_GAMEPAD_RIGHT_SHOULDER 0x0200
#define SGLP_XINPUT_GAMEPAD_A              0x1000
#define SGLP_XINPUT_GAMEPAD_B              0x2000
#define SGLP_XINPUT_GAMEPAD_X              0x4000
#define SGLP_XINPUT_GAMEPAD_Y              0x8000

typedef struct sglp_Win32SoundOutput {
    int32_t  samples_per_second;
    int32_t  running_sample_index;
    int32_t  bytes_per_sample;
    uint32_t secondary_buf_size;
    uint32_t safety_bytes;
} sglp_Win32SoundOutput;

//
// User32
//
static HMODULE sglp_global_user32dll;

typedef int SGLP_STDCALL sglp_TranslateMessage_t(const MSG *lpMsg);
static sglp_TranslateMessage_t *sglp_user32_TranslateMessage;

typedef LRESULT SGLP_STDCALL sglp_DispatchMessageA_t(const MSG *lpmsg);
static sglp_DispatchMessageA_t *sglp_user32_DispatchMessageA;

typedef int SGLP_STDCALL sglp_PeekMessageA_t(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
static sglp_PeekMessageA_t *sglp_user32_PeekMessageA;

typedef LRESULT SGLP_STDCALL sglp_DefWindowProcA_t(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static sglp_DefWindowProcA_t *sglp_user32_DefWindowProcA;

typedef ATOM SGLP_STDCALL sglp_RegisterClassA_t(const WNDCLASS *lpWndClass);
static sglp_RegisterClassA_t *sglp_user32_RegisterClassA;

typedef HWND SGLP_STDCALL SGLP_STDCALL sglp_CreateWindowExA_t(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static sglp_CreateWindowExA_t *sglp_user32_CreateWindowExA;

typedef int SGLP_STDCALL sglp_SetWindowPos_t(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
static sglp_SetWindowPos_t *sglp_user32_SetWindowPos;

typedef int SGLP_STDCALL sglp_GetWindowPlacement_t(HWND hWnd, WINDOWPLACEMENT *lpwndpl);
static sglp_GetWindowPlacement_t *sglp_user32_GetWindowPlacement;

typedef int SGLP_STDCALL sglp_SetWindowPlacement_t(HWND hWnd, const WINDOWPLACEMENT *lpwndpl);
static sglp_SetWindowPlacement_t *sglp_user32_SetWindowPlacement;

typedef HDC SGLP_STDCALL sglp_GetDC_t(HWND hWnd);
static sglp_GetDC_t *sglp_user32_GetDC;

typedef int SGLP_STDCALL sglp_GetCursorPos_t(LPPOINT lpPoint);
static sglp_GetCursorPos_t *sglp_user32_GetCursorPos;

typedef int SGLP_STDCALL sglp_ScreenToClient_t(HWND hWnd, LPPOINT lpPoint);
static sglp_ScreenToClient_t *sglp_user32_ScreenToClient;

typedef LONG SGLP_STDCALL sglp_GetWindowLongA_t(HWND hWnd, int nIndex);
static sglp_GetWindowLongA_t *sglp_user32_GetWindowLongA;

typedef LONG SGLP_STDCALL sglp_SetWindowLongA_t(HWND hWnd, int nIndex, LONG dwNewLong);
static sglp_SetWindowLongA_t *sglp_user32_SetWindowLongA;

typedef HMONITOR SGLP_STDCALL sglp_MonitorFromWindow_t(HWND hwnd, DWORD dwFlags);
static sglp_MonitorFromWindow_t *sglp_user32_MonitorFromWindow;

typedef sglp_Bool SGLP_STDCALL sglp_GetMonitorInfoA_t(HMONITOR hMonitor, LPMONITORINFO lpmi);
static sglp_GetMonitorInfoA_t *sglp_user32_GetMonitorInfoA;

typedef int SGLP_STDCALL sglp_GetSystemMetrics_t(int nIndex);
static sglp_GetSystemMetrics_t *sglp_user32_GetSystemMetrics;

//
// Gdi32
//
static HMODULE sglp_global_gdi32dll;

typedef int SGLP_STDCALL sglp_ChoosePixelFormat_t(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd);
static sglp_ChoosePixelFormat_t *sglp_gdi_ChoosePixelFormat;

typedef int SGLP_STDCALL sglp_DescribePixelFormat_t(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd);
static sglp_DescribePixelFormat_t *sglp_gdi_DescribePixelFormat;

typedef sglp_Bool SGLP_STDCALL sglp_SetPixelFormat_t(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR *ppfd);
static sglp_SetPixelFormat_t *sglp_gdi_SetPixelFormat;

typedef sglp_Bool SGLP_STDCALL sglp_SwapBuffers_t(HDC hdc);
static sglp_SwapBuffers_t *sglp_gdi_SwapBuffers;

//
// Direct Sound
//
static sglp_Bool sglp_init_dsound(HWND win, int32_t samples_per_second, int32_t buf_size, LPDIRECTSOUNDBUFFER *secondary_buf) {
    sglp_Bool res = SGLP_FALSE, primary_buf_set = SGLP_FALSE, secondary_buf_set = SGLP_FALSE;

    HMODULE hdsound = LoadLibraryA("dsound.dll");
    if(hdsound) {
        typedef HRESULT SGLP_STDCALL DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
        DirectSoundCreate *direct_sound_create = (DirectSoundCreate *)GetProcAddress(hdsound, "DirectSoundCreate");

        LPDIRECTSOUND dsound = 0;
        if((direct_sound_create) && (SUCCEEDED(direct_sound_create(0, &dsound, 0)))) {
            WAVEFORMATEX wav_fmt = {0};
            if(dsound) {
                // Primary buffer, which just gets a handle to the "hardware".
                if(SUCCEEDED(IDirectSound_SetCooperativeLevel(dsound, win, DSSCL_PRIORITY))) {
                    LPDIRECTSOUNDBUFFER primary_buf;
                    DSBUFFERDESC buf_desc = {0};
                    buf_desc.dwSize = sizeof(buf_desc);
                    buf_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

                    if(SUCCEEDED(IDirectSound_CreateSoundBuffer(dsound, &buf_desc, &primary_buf, 0))) {
                        wav_fmt.wFormatTag = WAVE_FORMAT_PCM;
                        wav_fmt.nChannels = 2;
                        wav_fmt.nSamplesPerSec = samples_per_second;
                        wav_fmt.wBitsPerSample = 16;
                        wav_fmt.nBlockAlign = (wav_fmt.nChannels * wav_fmt.wBitsPerSample) / 8;
                        wav_fmt.nAvgBytesPerSec = wav_fmt.nSamplesPerSec * wav_fmt.nBlockAlign;
                        wav_fmt.cbSize = 0;
                        if(SUCCEEDED(IDirectSoundBuffer_SetFormat(primary_buf, &wav_fmt))) {
                            primary_buf_set = SGLP_TRUE;
                        }
                    }
                }
            }

            // Secondary buffer, which is the real buffer we'll use.
            {
                DSBUFFERDESC buf_desc = {0};
                buf_desc.dwSize = sizeof(buf_desc);
                buf_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
#if INTERNAL
                buf_desc.dwFlags |= DSBCAPS_GLOBALFOCUS;
#endif
                buf_desc.dwBufferBytes = buf_size;
                buf_desc.lpwfxFormat = &wav_fmt;

                if(SUCCEEDED(IDirectSound_CreateSoundBuffer(dsound, &buf_desc, secondary_buf, 0))) {
                    secondary_buf_set = SGLP_TRUE;
                }
            }
        }

        FreeLibrary(hdsound);
    }

    res = ((secondary_buf_set) && (primary_buf_set));

    return(res);
}

//
// XInput.
//
typedef DWORD SGLP_STDCALL sglp_XInputGetState_t(DWORD dwUserIndex, SGLP_XINPUT_STATE *pState);
static sglp_XInputGetState_t *sglp_xinput_get_state;

typedef DWORD SGLP_STDCALL sglp_XInputSetState_t(DWORD dwUserIndex, SGLP_XINPUT_VIBRATION *pVibration);
static sglp_XInputSetState_t *sglp_xinput_set_state;

static sglp_Bool sglp_init_xinput(void) {
    sglp_Bool res = SGLP_FALSE;
    HMODULE hxinput = 0;

    if(!hxinput) { hxinput = LoadLibraryA("Xinput1_3.dll");   }
    if(!hxinput) { hxinput = LoadLibraryA("Xinput1_4.dll");   }
    if(!hxinput) { hxinput = LoadLibraryA("Xinput9_1_0.dll"); }
    if(!hxinput) { hxinput = LoadLibraryA("Xinputuap.dll");   }

    if(hxinput) {
        sglp_xinput_get_state = (sglp_XInputGetState_t *)GetProcAddress(hxinput, "XInputGetState");
        sglp_xinput_set_state = (sglp_XInputSetState_t *)GetProcAddress(hxinput, "XInputSetState");
        if((sglp_xinput_get_state) && (sglp_xinput_set_state)) {
            res = SGLP_TRUE;
        }
    }

    return(res);
}

//
// OpenGL
//
#define SGLP_WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define SGLP_WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define SGLP_WGL_CONTEXT_LAYER_PLANE_ARB               0x2093
#define SGLP_WGL_CONTEXT_FLAGS_ARB                     0x2094
#define SGLP_WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define SGLP_WGL_CONTEXT_DEBUG_BIT_ARB                 0x0001
#define SGLP_WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    0x0002

#define SGLP_WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define SGLP_WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

static sglp_Bool sglp_win32_init_opengl(HWND win) {
    typedef PROC  SGLP_STDCALL wglGetProcAddress_t(LPCSTR lpszProc);
    typedef HGLRC SGLP_STDCALL wglCreateContext_t(HDC hdc);
    typedef sglp_Bool  SGLP_STDCALL wglMakeCurrent_t(HDC hdc, HGLRC hglrc);
    typedef sglp_Bool  SGLP_STDCALL wglDeleteContext_t(HGLRC hglrc);
    typedef HGLRC SGLP_STDCALL wglCreateContextAttribsArb_t(HDC hDC, HGLRC hShareContext, const int *attribList);
    typedef sglp_Bool  SGLP_STDCALL wglSwapIntervalExt_t(int interval);

    sglp_Bool res = SGLP_FALSE;

    HDC dc = sglp_user32_GetDC(win);
    if(dc) {
        HMODULE hopengl32 = LoadLibraryA("opengl32.dll");
        if(!hopengl32) {
            SGLP_ASSERT(0);
        } else {
            wglGetProcAddress_t *gl32_wglGetProcAddress = (wglGetProcAddress_t *)GetProcAddress(hopengl32, "wglGetProcAddress");
            wglCreateContext_t  *gl32_wglCreateContext  = (wglCreateContext_t *) GetProcAddress(hopengl32, "wglCreateContext");
            wglMakeCurrent_t    *gl32_wglMakeCurrent    = (wglMakeCurrent_t *)   GetProcAddress(hopengl32, "wglMakeCurrent");
            wglDeleteContext_t  *gl32_wglDeleteContext  = (wglDeleteContext_t *) GetProcAddress(hopengl32, "wglDeleteContext");
            if((gl32_wglGetProcAddress) && (gl32_wglCreateContext) && (gl32_wglMakeCurrent) && (gl32_wglDeleteContext)) {
                int32_t suggested_pixel_fmt_index;
                PIXELFORMATDESCRIPTOR desired_pixel_fmt = {0}, suggested_pixel_fmt = {0};
                HGLRC opengl_rc;

                desired_pixel_fmt.nSize = sizeof(desired_pixel_fmt);
                desired_pixel_fmt.nVersion = 1;
                desired_pixel_fmt.iPixelType = PFD_TYPE_RGBA;
                desired_pixel_fmt.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
                desired_pixel_fmt.cColorBits = 24; // TODO(Jonny): Should this be 32... even though MSDN says EXCLUDING alpha??
                desired_pixel_fmt.cAlphaBits = 8;

                suggested_pixel_fmt_index = sglp_gdi_ChoosePixelFormat(dc, &desired_pixel_fmt);
                sglp_gdi_DescribePixelFormat(dc, suggested_pixel_fmt_index, sizeof(suggested_pixel_fmt), &suggested_pixel_fmt);
                sglp_gdi_SetPixelFormat(dc, suggested_pixel_fmt_index, &suggested_pixel_fmt);

                opengl_rc = gl32_wglCreateContext(dc);
                if(opengl_rc) {
                    if(gl32_wglMakeCurrent(dc, opengl_rc)) {
                        wglCreateContextAttribsArb_t *wglCreateContextAttribsARB = (wglCreateContextAttribsArb_t *)gl32_wglGetProcAddress("wglCreateContextAttribsARB");
                        if(wglCreateContextAttribsARB) {
                            int32_t opengl_attribs[] = {SGLP_WGL_CONTEXT_MAJOR_VERSION_ARB, 3, SGLP_WGL_CONTEXT_MINOR_VERSION_ARB, 0, SGLP_WGL_CONTEXT_FLAGS_ARB,
#if INTERNAL
                                                        SGLP_WGL_CONTEXT_DEBUG_BIT_ARB
#else
                                                        0
#endif
                                                        , SGLP_WGL_CONTEXT_PROFILE_MASK_ARB, SGLP_WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, 0
                                                       };
                            HGLRC shared_context = 0;
                            HGLRC modern_glrc = wglCreateContextAttribsARB(dc, shared_context, opengl_attribs);
                            if(modern_glrc) {
                                if(gl32_wglMakeCurrent(dc, modern_glrc)) {
                                    wglSwapIntervalExt_t *OpenGL32wglSwapIntervalEXT = (wglSwapIntervalExt_t *)gl32_wglGetProcAddress("wglSwapIntervalEXT");

                                    gl32_wglDeleteContext(opengl_rc);
                                    opengl_rc = modern_glrc;

                                    if(OpenGL32wglSwapIntervalEXT) {
                                        OpenGL32wglSwapIntervalEXT(1);
                                    }

                                    // Load all the OpenGl functions now.
                                    {
                                        sglp_Bool all_gl_funcs_loaded = SGLP_TRUE;

#define SGLP_OPENGL_LOAD_(gl_func, gl_func_t, gl_func_str) sglp_global_opengl->gl_func = (sglp_##gl_func_t *)GetProcAddress(hopengl32, gl_func_str); if(!sglp_global_opengl->gl_func) { sglp_global_opengl->gl_func = (sglp_##gl_func_t *)gl32_wglGetProcAddress(gl_func_str); } if(all_gl_funcs_loaded) { all_gl_funcs_loaded = (sglp_global_opengl->gl_func) != 0; } else { SGLP_ASSERT(0); }
#define SGLP_OPENGL_LOAD(gl_func) do { SGLP_OPENGL_LOAD_(gl_func, gl_func##_t, #gl_func) } while(0)

                                        SGLP_OPENGL_LOAD(glBindTexture);
                                        SGLP_OPENGL_LOAD(glClear);
                                        SGLP_OPENGL_LOAD(glClearColor);
                                        SGLP_OPENGL_LOAD(glDrawArrays);
                                        SGLP_OPENGL_LOAD(glGetError);
                                        SGLP_OPENGL_LOAD(glTexImage2D);
                                        SGLP_OPENGL_LOAD(glTexParameteri);
                                        SGLP_OPENGL_LOAD(glGetString);
                                        SGLP_OPENGL_LOAD(glViewport);

                                        SGLP_OPENGL_LOAD(glCreateShader);
                                        SGLP_OPENGL_LOAD(glShaderSource);
                                        SGLP_OPENGL_LOAD(glCompileShader);
                                        SGLP_OPENGL_LOAD(glGetShaderiv);
                                        SGLP_OPENGL_LOAD(glCreateProgram);
                                        SGLP_OPENGL_LOAD(glAttachShader);
                                        SGLP_OPENGL_LOAD(glBindAttribLocation);
                                        SGLP_OPENGL_LOAD(glLinkProgram);
                                        SGLP_OPENGL_LOAD(glUseProgram);
                                        SGLP_OPENGL_LOAD(glGenBuffers);
                                        SGLP_OPENGL_LOAD(glBindBuffer);
                                        SGLP_OPENGL_LOAD(glBufferData);
                                        SGLP_OPENGL_LOAD(glVertexAttribPointer);
                                        SGLP_OPENGL_LOAD(glEnableVertexAttribArray);
                                        SGLP_OPENGL_LOAD(glUniform1i);
                                        SGLP_OPENGL_LOAD(glUniform2f);
                                        SGLP_OPENGL_LOAD(glUniform4f);
                                        SGLP_OPENGL_LOAD(glUniformMatrix4fv);
                                        SGLP_OPENGL_LOAD(glGetUniformLocation);
                                        SGLP_OPENGL_LOAD(glIsShader);
                                        SGLP_OPENGL_LOAD(glGetProgramiv);
                                        SGLP_OPENGL_LOAD(glGetProgramInfoLog);
                                        SGLP_OPENGL_LOAD(glGetShaderInfoLog);

                                        SGLP_OPENGL_LOAD(glGenVertexArrays);
                                        SGLP_OPENGL_LOAD(glBindVertexArray);

#undef SGLP_OPENGL_LOAD_
#undef SGLP_OPENGL_LOAD

                                        res = all_gl_funcs_loaded;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            FreeLibrary(hopengl32);
        }
    }

    return(res);
}

static WINDOWPLACEMENT sglp_global_win_pos = {sizeof(sglp_global_win_pos)};
static void sglp_win32_toggle_fullscreen(HWND win) {
    uint32_t style = sglp_user32_GetWindowLongA(win, GWL_STYLE);

    if(style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        if((sglp_user32_GetWindowPlacement(win, &sglp_global_win_pos)) &&
                (sglp_user32_GetMonitorInfoA(sglp_user32_MonitorFromWindow(win, MONITOR_DEFAULTTOPRIMARY), &monitor_info))) {
            sglp_user32_SetWindowLongA(win, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            sglp_user32_SetWindowPos(win, HWND_TOP,
                                     monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                                     monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                                     monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        sglp_user32_SetWindowLongA(win, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        sglp_user32_SetWindowPlacement(win, &sglp_global_win_pos);
        sglp_user32_SetWindowPos(win, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

//
// DLL Loading.
//
static sglp_Bool sglp_load_user32dll(void) {
    sglp_Bool res = SGLP_FALSE;
    sglp_global_user32dll = LoadLibraryA("user32.dll");

    if(sglp_global_user32dll) {
        sglp_Bool funcs_loaded = SGLP_TRUE;

#define SGLP_USER32_LOAD_(func, func_t, func_str) func = (sglp_##func_t *)GetProcAddress(sglp_global_user32dll, func_str); if(funcs_loaded) { funcs_loaded = (func) != 0; } else { SGLP_ASSERT(0); }
#define SGLP_USER32_LOAD(func) SGLP_USER32_LOAD_(sglp_user32_##func, func##_t, #func)

        SGLP_USER32_LOAD(TranslateMessage);
        SGLP_USER32_LOAD(DispatchMessageA);
        SGLP_USER32_LOAD(PeekMessageA);
        SGLP_USER32_LOAD(DefWindowProcA);
        SGLP_USER32_LOAD(RegisterClassA);
        SGLP_USER32_LOAD(CreateWindowExA);
        SGLP_USER32_LOAD(SetWindowPos);
        SGLP_USER32_LOAD(GetWindowPlacement);
        SGLP_USER32_LOAD(SetWindowPlacement);
        SGLP_USER32_LOAD(GetDC);
        SGLP_USER32_LOAD(GetCursorPos);
        SGLP_USER32_LOAD(ScreenToClient);
        SGLP_USER32_LOAD(GetWindowLongA);
        SGLP_USER32_LOAD(SetWindowLongA);
        SGLP_USER32_LOAD(MonitorFromWindow);
        SGLP_USER32_LOAD(GetMonitorInfoA);
        SGLP_USER32_LOAD(GetSystemMetrics);

#undef SGLP_USER32_LOAD_
#undef SGLP_USER32_LOAD

        res = funcs_loaded;
    } else {
        SGLP_ASSERT(0);
    }

    return(res);
}

static sglp_Bool sglp_load_gdi32dll(void) {
    sglp_Bool res = SGLP_FALSE;
    sglp_global_gdi32dll = LoadLibraryA("gdi32.dll");
    if(sglp_global_gdi32dll) {
        sglp_Bool funcs_loaded = SGLP_TRUE;

#define SGLP_GDI_LOAD_(func, func_t, func_str) func = (sglp_##func_t *)GetProcAddress(sglp_global_gdi32dll, func_str); if(funcs_loaded) { funcs_loaded = (func) != 0; } else { SGLP_ASSERT(0); }
#define SGLP_GDI_LOAD(func) SGLP_GDI_LOAD_(sglp_gdi_##func, func##_t, #func)

        SGLP_GDI_LOAD(ChoosePixelFormat);
        SGLP_GDI_LOAD(DescribePixelFormat);
        SGLP_GDI_LOAD(SetPixelFormat);
        SGLP_GDI_LOAD(SwapBuffers);

#undef SGLP_GDI_LOAD_
#undef SGLP_GDI_LOAD

        res = funcs_loaded;
    } else {
        SGLP_ASSERT(0);
    }

    return(res);
}

//
// Threading
//
typedef struct WorkQueueEntry {
    void (*callback)(void *data);
    void *e;
} WorkQueueEntry;

typedef struct WorkQueue {
    int32_t volatile goal;
    int32_t volatile cnt;

    int32_t volatile next_entry_to_write;
    int32_t volatile next_entry_to_read;

    void *hsem;
    WorkQueueEntry entries[256]; // TODO(Jonny): Should I make this a Linked List?? Or a dynamically resizing array??
} WorkQueue;
static WorkQueue sglp_global_work_queue;

static void sglp_win32_add_work_queue_entry(void *e, void (*callback)(void *data)) {
    int32_t new_next_entry_to_write = (sglp_global_work_queue.next_entry_to_write + 1) % SGLP_ARRAY_COUNT(sglp_global_work_queue.entries);

    WorkQueueEntry *entry = sglp_global_work_queue.entries + sglp_global_work_queue.next_entry_to_write;
    entry->callback = callback;
    entry->e = e;

    ++sglp_global_work_queue.goal;

    _WriteBarrier(); _mm_sfence();

    sglp_global_work_queue.next_entry_to_write = new_next_entry_to_write;
    ReleaseSemaphore(sglp_global_work_queue.hsem, 1, 0);
}

static sglp_Bool sglp_do_next_work_queue_entry(WorkQueue *work_queue) {
    sglp_Bool res = SGLP_FALSE;

    int32_t original_next_entry_to_read = work_queue->next_entry_to_read;
    int32_t new_new_entry_to_read = (original_next_entry_to_read + 1) % SGLP_ARRAY_COUNT(work_queue->entries);
    if(original_next_entry_to_read != work_queue->next_entry_to_write) {
        int32_t index = InterlockedCompareExchange((long volatile *)&work_queue->next_entry_to_read,
                                                   new_new_entry_to_read, original_next_entry_to_read);

        if(index == original_next_entry_to_read) {
            WorkQueueEntry entry = work_queue->entries[index];
            entry.callback(entry.e);
            InterlockedIncrement((volatile long *)&work_queue->cnt);
        }
    } else {
        res = SGLP_TRUE;
    }

    return(res);
}

static void sglp_win32_complete_all_work(void) {
    while(sglp_global_work_queue.goal != sglp_global_work_queue.cnt) {
        sglp_do_next_work_queue_entry(&sglp_global_work_queue);
    }

    sglp_global_work_queue.goal = 0;
    sglp_global_work_queue.cnt = 0;
}

static DWORD SGLP_STDCALL sglp_thread_proc(void *p) {
    WorkQueue *work_queue = (WorkQueue *)p;
    for(;;) {
        if(sglp_do_next_work_queue_entry(work_queue)) {
            WaitForSingleObjectEx(work_queue->hsem, INFINITE, 0);
        }
    }
}

//
// sglp_File IO
//
static uint32_t sglp_safe_truncate_64_to_32(uint64_t v) {
    uint32_t res;
    SGLP_ASSERT(v <= 0xFFFFFFFF);
    res = (uint32_t)v;

    return(res);
}

static sglp_File sglp_win32_read_file(sglp_API *api, char const *fname) {
    sglp_File res = {0};

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fsize;
        if(GetFileSizeEx(fhandle, &fsize)) {
            DWORD fsize32 = sglp_safe_truncate_64_to_32((uintptr_t)fsize.QuadPart);
            res.e = (uint8_t *)api->os_malloc(fsize32);
            if(res.e) {
                DWORD bytes_read;
                if((ReadFile(fhandle, res.e, fsize32, &bytes_read, 0)) && (fsize32 == bytes_read)) {
                    res.size = fsize32;
                }
            }
        }

        CloseHandle(fhandle);
    }

    return(res);
}

static sglp_File sglp_win32_read_file_and_null_terminate(sglp_API *api, char const *fname) {
    sglp_File res = {0};

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fsize;
        if(GetFileSizeEx(fhandle, &fsize)) {
            DWORD fsize32 = sglp_safe_truncate_64_to_32((uintptr_t)fsize.QuadPart);
            res.e = (uint8_t *)api->os_malloc(fsize32 + 1);
            if(res.e) {
                DWORD bytes_read;
                if((ReadFile(fhandle, res.e, fsize32, &bytes_read, 0)) && (fsize32 == bytes_read)) {
                    res.size = fsize32;
                    res.e[res.size] = 0;
                }
            }
        }

        CloseHandle(fhandle);
    }

    return(res);
}

static uint64_t sglp_win32_get_processor_timestamp(void) {
    uint64_t res = __rdtsc();

    return(res);
}

//
// Memory
//
static void *sglp_win32_malloc(uintptr_t size) {
    void *res = HeapAlloc(GetProcessHeap(), 0x00000008/*HEAP_ZERO_MEMORY*/, size);

    return(res);
}

static void *sglp_win32_realloc(void *ptr, uintptr_t size) {
    void *res;

    if(ptr) {
        res = HeapReAlloc(GetProcessHeap(), 0x00000008/*HEAP_ZERO_MEMORY*/, ptr, size);
    } else {
        res = HeapAlloc(GetProcessHeap(), 0x00000008/*HEAP_ZERO_MEMORY*/, size);
    }

    return(res);
}

static void sglp_win32_free(void *ptr) {
    if(ptr) {
        sglp_Bool success = HeapFree(GetProcessHeap(), 0, ptr);
        SGLP_ASSERT(success);
    }
}

//
// Win32 crap.
//
static LARGE_INTEGER sglp_win32_get_wall_clock(void) {
    LARGE_INTEGER res = {0};
    QueryPerformanceCounter(&res);

    return(res);
}

static float win32_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end,
                                       int64_t perf_cnt_freq) {
    float res = (float)(end.QuadPart - start.QuadPart) / (float)perf_cnt_freq;

    return(res);
}

static sglp_Key sglp_win_key_to_sgl_key(WPARAM k) {
    sglp_Key res = sglp_key_unknown;
    switch(k) {
        case VK_CONTROL: { res = sglp_key_ctrl;   } break;
        case VK_SHIFT:   { res = sglp_key_shift;  } break;
        //case VK_ALT:     { res = sglp_key_alt;    } break;
        case VK_SPACE:   { res = sglp_key_space;  } break;
        case VK_ESCAPE:  { res = sglp_key_escape; } break;

        case VK_LEFT:    { res = sglp_key_left;  } break;
        case VK_RIGHT:   { res = sglp_key_right; } break;
        case VK_UP:      { res = sglp_key_up;    } break;
        case VK_DOWN:    { res = sglp_key_down;  } break;

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y':case 'Z': {
            res = (sglp_Key)k;
        } break;
    }

    return(res);
}

static void sglp_win32_process_pending_messages(HWND wnd, float *key, sglp_Bool *quit) {
    MSG msg;
    while(sglp_user32_PeekMessageA(&msg, wnd, 0, 0, PM_REMOVE)) {
        switch(msg.message) {
            case WM_QUIT: {
                *quit = SGLP_TRUE;
            }  break;

            case WM_KEYDOWN: { key[sglp_win_key_to_sgl_key(msg.wParam)] = 1.0f; } break;
            case WM_KEYUP:   { key[sglp_win_key_to_sgl_key(msg.wParam)] = 0.0f; } break;

            default: {
                sglp_user32_TranslateMessage(&msg);
                sglp_user32_DispatchMessageA(&msg);
            } break;
        }
    }
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_code) {
    int i;
    sglp_API api = {0};

    // Setup all the .
    api.free_file = sglp_free_file;
    api.read_file = sglp_win32_read_file;
    api.read_file_and_null_terminate = sglp_win32_read_file_and_null_terminate;

    api.get_processor_timestamp = sglp_win32_get_processor_timestamp;

    api.add_work_queue_entry = sglp_win32_add_work_queue_entry;
    api.complete_all_work = sglp_win32_complete_all_work;

    api.os_malloc = sglp_win32_malloc;
    api.os_realloc = sglp_win32_realloc;
    api.os_free = sglp_win32_free;

    sglp_global_opengl = &api.gl;

    if((sglp_load_user32dll()) && (sglp_load_gdi32dll())) {
        int monitor_refresh_hz, game_update_hz;
        SYSTEM_INFO system_info;
        LARGE_INTEGER perf_cnt_freq_res;
        int64_t perf_cnt_freq;
        WNDCLASS wnd_class = {0};
        float target_seconds_per_frame, target_ms_per_frame;

        // Default sglp_API settings.
        GetSystemInfo(&system_info);

        // Frame rate.
        monitor_refresh_hz = 60; // TODO(Jonny): Read from hardware.
        api.settings.frame_rate = monitor_refresh_hz;

        // Thread count.
        api.settings.thread_cnt = (system_info.dwNumberOfProcessors > 0) ? system_info.dwNumberOfProcessors : 1;

        api.settings.win_width = sglp_user32_GetSystemMetrics(SM_CXSCREEN);
        api.settings.win_height = sglp_user32_GetSystemMetrics(SM_CYSCREEN);

        sglp_platform_setup_settings_callback(&api.settings);

        QueryPerformanceFrequency(&perf_cnt_freq_res);
        perf_cnt_freq = perf_cnt_freq_res.QuadPart;

        // TODO(Jonny): Replace with WNDCLASSEX??
        wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wnd_class.hInstance = instance;
        wnd_class.lpszClassName = TEXT("Frozen_Game");
        wnd_class.lpfnWndProc = sglp_user32_DefWindowProcA;

        game_update_hz = (api.settings.frame_rate) ? api.settings.frame_rate : api.settings.frame_rate;
        target_seconds_per_frame = 1.0f / (float)game_update_hz;
        target_ms_per_frame = (1.0f / api.settings.frame_rate) * 1000.0f;

        // To make the frame rate more granular.
        {
            HMODULE winmmdll = LoadLibraryA("winmm.dll");
            if(winmmdll) {
                typedef MMRESULT TimeBeginPeriod_t(uint32_t uPeriod);
                TimeBeginPeriod_t *winm_timeBeginPeriod = (TimeBeginPeriod_t *)GetProcAddress(winmmdll, "timeBeginPeriod");
                if(winm_timeBeginPeriod) {
                    winm_timeBeginPeriod(1);
                }

                FreeLibrary(winmmdll);
            }
        }

        if(sglp_user32_RegisterClassA(&wnd_class)) {
            char const *window_title = (api.settings.window_title) ? api.settings.window_title : "Game";
            HWND win;

            if(!api.settings.win_width) {
                api.settings.win_width = 1920;
            }

            if(!api.settings.win_height) {
                api.settings.win_height = 1080;
            }

            win = sglp_user32_CreateWindowExA(0, wnd_class.lpszClassName, window_title,
                                              WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
                                              api.settings.win_width, api.settings.win_height, 0, 0, instance, 0);
            if((win) && (win != INVALID_HANDLE_VALUE)) {
                sglp_Bool should_play_snd, should_allow_controller;
                sglp_Win32SoundOutput snd_output = {0};
                LPDIRECTSOUNDBUFFER secondary_buffer = 0;
                int16_t *snd_samples = 0;

                if(api.settings.fullscreen) {
                    sglp_win32_toggle_fullscreen(win);
                }

                snd_output.samples_per_second = 48000;
                snd_output.bytes_per_sample = sizeof(int16_t) * 2;
                snd_output.secondary_buf_size = snd_output.samples_per_second * snd_output.bytes_per_sample;
                snd_output.safety_bytes =
                    (int32_t)(((float)snd_output.samples_per_second * (float)snd_output.bytes_per_sample / game_update_hz) / 3.0f);

                should_play_snd = sglp_init_dsound(win, snd_output.samples_per_second,
                                                   snd_output.secondary_buf_size, &secondary_buffer);

                if(should_play_snd) {
                    if(!secondary_buffer) {
                        should_play_snd = SGLP_FALSE;
                    } else {
                        // Clear the sound buffer.
                        void *region1, *region2;
                        DWORD region1_size, region2_size;

                        if(SUCCEEDED(IDirectSoundBuffer_Lock(secondary_buffer, 0, snd_output.secondary_buf_size,
                                                             &region1, &region1_size, &region2, &region2_size, 0))) {
                            uint8_t *DestSample;

                            DestSample = (uint8_t *)region1;
                            for(i = 0; (i < region1_size); ++i) {
                                *DestSample++ = 0;
                            }

                            DestSample = (uint8_t *)region2;
                            for(i = 0; (i < region2_size); ++i) {
                                *DestSample++ = 0;
                            }

                            IDirectSoundBuffer_Unlock(secondary_buffer, region1, region1_size, region2, region2_size);
                        }

                        IDirectSoundBuffer_Play(secondary_buffer, 0, 0, DSBPLAY_LOOPING);

                        // Sample size plus a max possible overrun.
                        snd_samples = (int16_t *)sglp_win32_malloc(snd_output.secondary_buf_size + ((2 * 8) * sizeof(int16_t)));
                    }
                }

                should_allow_controller = sglp_init_xinput();

                sglp_global_work_queue.hsem = CreateSemaphore(0, 0, api.settings.thread_cnt, 0);

                for(i = 0; (i < api.settings.thread_cnt); ++i) {
                    HANDLE hthread = CreateThread(0, 0, sglp_thread_proc, &sglp_global_work_queue, 0, 0);
                    SGLP_ASSERT((hthread) && (hthread != INVALID_HANDLE_VALUE));

                    CloseHandle(hthread);
                }

                if(sglp_win32_init_opengl(win)) {
                    api.permanent_memory = sglp_win32_malloc(api.settings.permanent_memory_size);
                    if(!api.permanent_memory) {
                        SGLP_ASSERT(0);
                    } else {
                        LARGE_INTEGER last_counter = sglp_win32_get_wall_clock();
                        LARGE_INTEGER flip_wall_clock = sglp_win32_get_wall_clock();

                        float ms_per_frame = 16.6666f;
                        sglp_Bool snd_is_valid = SGLP_FALSE;
                        sglp_Bool allow_snd = SGLP_TRUE;

                        sglp_setup(&api, api.settings.max_no_of_sounds);

                        api.init_game = SGLP_TRUE;
                        while(!api.quit) {
                            sglp_win32_process_pending_messages(win, api.key, &api.quit);

                            //
                            // XInput.
                            //
                            if(should_allow_controller) {
                                for(i = 0; (i < /*XUSER_MAX_COUNT*/4); ++i) {
                                    SGLP_XINPUT_STATE controller_state;
                                    if((sglp_xinput_get_state) && (sglp_xinput_get_state(i, &controller_state) == ERROR_SUCCESS)) {
                                        // TODO(Jonny): Make sure controller_state.dwPacketNumber doesn't increment too quickly...
                                        api.key[sglp_dpad_up]        = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_DPAD_UP);
                                        api.key[sglp_dpad_down]      = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_DPAD_DOWN);
                                        api.key[sglp_dpad_left]      = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_DPAD_LEFT);
                                        api.key[sglp_dpad_right]     = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_DPAD_RIGHT);
                                        api.key[start]          = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_START);
                                        api.key[back]           = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_BACK);
                                        api.key[left_shoulder]  = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_LEFT_SHOULDER);
                                        api.key[right_shoulder] = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_RIGHT_SHOULDER);
                                        api.key[left_thumb]     = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_LEFT_THUMB);
                                        api.key[right_thumb]    = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_RIGHT_THUMB);
                                        api.key[sglp_controller_a]   = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_A);
                                        api.key[sglp_controller_b]   = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_B);
                                        api.key[sglp_controller_x]   = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_X);
                                        api.key[sglp_controller_y]   = (controller_state.Gamepad.wButtons & SGLP_XINPUT_GAMEPAD_Y);

                                        // Sticks
                                        {
                                            int16_t left_stickx  = controller_state.Gamepad.sThumbLX;
                                            int16_t left_sticky  = controller_state.Gamepad.sThumbLY;
                                            int16_t right_stickx = controller_state.Gamepad.sThumbRX;
                                            int16_t right_sticky = controller_state.Gamepad.sThumbRY;

                                            api.key[sglp_left_stick_x] = 0.0f;
                                            api.key[sglp_left_stick_y] = 0.0f;

                                            if(left_stickx > 0) {
                                                api.key[sglp_left_stick_x] = SGLP_NORMALISE(left_stickx, 0, 0xFFFF);
                                            } else if (left_stickx < 0) {
                                                api.key[sglp_left_stick_x] = -SGLP_NORMALISE(left_stickx, 0, 0xFFFF);
                                            }

                                            if(left_stickx > 0) {
                                                api.key[sglp_left_stick_y] = SGLP_NORMALISE(left_sticky, 0, 0xFFFF);
                                            } else if (left_stickx < 0) {
                                                api.key[sglp_left_stick_y] = -SGLP_NORMALISE(left_sticky, 0, 0xFFFF);
                                            }
                                        }
                                    } else {
                                        // Controller not available.
                                    }
                                }
                            }

                            api.dt = ms_per_frame;
                            sglp_platform_update_and_render_callback(&api);
                            api.init_game = SGLP_FALSE;

                            sglp_gdi_SwapBuffers(sglp_user32_GetDC(win));

                            // Output Sound.
                            if(should_play_snd) {
                                DWORD play_cursor, write_cursor;
                                if(IDirectSoundBuffer_GetCurrentPosition(secondary_buffer, &play_cursor, &write_cursor) == DS_OK) {
                                    sglp_SoundOutputBuffer snd_buf = {0};
                                    uint32_t byte_to_lock, expected_snd_bytes_per_frame, safe_write_cursor, target_cursor, bytes_to_write;

                                    if(!snd_is_valid) {
                                        snd_output.running_sample_index = write_cursor / snd_output.bytes_per_sample;
                                        snd_is_valid = SGLP_TRUE;
                                    }

                                    byte_to_lock =
                                        (snd_output.running_sample_index * snd_output.bytes_per_sample) % snd_output.secondary_buf_size;
                                    expected_snd_bytes_per_frame =
                                        (DWORD)(((float)snd_output.samples_per_second * snd_output.bytes_per_sample) / game_update_hz);
                                    safe_write_cursor = write_cursor;
                                    if(safe_write_cursor < play_cursor) {
                                        safe_write_cursor += snd_output.secondary_buf_size;
                                    }

                                    SGLP_ASSERT(safe_write_cursor >= play_cursor);
                                    safe_write_cursor += snd_output.safety_bytes;

                                    target_cursor =
                                        (write_cursor + expected_snd_bytes_per_frame + snd_output.safety_bytes) % snd_output.secondary_buf_size;
                                    if(byte_to_lock > target_cursor) {
                                        bytes_to_write = (snd_output.secondary_buf_size - byte_to_lock) + target_cursor;
                                    } else {
                                        bytes_to_write = target_cursor - byte_to_lock;
                                    }

                                    snd_buf.samples_per_second = snd_output.samples_per_second;

                                    snd_buf.sample_cnt = SGLP_ALIGN8(bytes_to_write / snd_output.bytes_per_sample);
                                    bytes_to_write = snd_buf.sample_cnt * snd_output.bytes_per_sample;
                                    snd_buf.samples = snd_samples;
                                    sglp_output_playing_sounds(&api, &snd_buf);

                                    if(allow_snd) {
                                        // Fill the Sound Buffer.
                                        void *region1, *region2;
                                        DWORD region1_size, region2_size;
                                        if(SUCCEEDED(IDirectSoundBuffer_Lock(secondary_buffer, byte_to_lock, bytes_to_write,
                                                                             &region1, &region1_size, &region2, &region2_size,
                                                                             0))) {
                                            int16_t *DestSample;
                                            int16_t *source_sample = snd_buf.samples;

                                            DestSample = (int16_t *)region1;
                                            for(i = 0; (i < region1_size / snd_output.bytes_per_sample); ++i) {
                                                *DestSample++ = *source_sample++;
                                                *DestSample++ = *source_sample++;
                                                ++snd_output.running_sample_index;
                                            }

                                            DestSample = (int16_t *)region2;
                                            for(i = 0; (i < region2_size / snd_output.bytes_per_sample); ++i) {
                                                *DestSample++ = *source_sample++;
                                                *DestSample++ = *source_sample++;
                                                ++snd_output.running_sample_index;
                                            }

                                            /*
                                            auto fill_buffer=[&](void *region, uint32_t region_sample_cnt) {
                                                int16_t *DestSample = (int16_t *)region;
                                                for(int32_t i = 0; (i < region_sample_cnt); ++i) {
                                                    *DestSample++ = *source_sample++;
                                                    *DestSample++ = *source_sample++;
                                                    ++snd_output.running_sample_index;
                                                }

                                            };
                                            fill_buffer(region1, region1_size / snd_output.bytes_per_sample);
                                            fill_buffer(region2, region2_size / snd_output.bytes_per_sample);
                                            */
                                            IDirectSoundBuffer_Unlock(secondary_buffer, region1, region1_size, region2, region2_size);
                                        }
                                    }
                                } else {
                                    snd_is_valid = SGLP_FALSE;
                                }
                            }

                            // Frame rate stuff.
                            {
                                float seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter,
                                                                                            sglp_win32_get_wall_clock(),
                                                                                            perf_cnt_freq);
                                if(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                    float test_seconds_elapsed_for_frame;
                                    DWORD sleepms = (DWORD)(1000.0f * (target_seconds_per_frame - seconds_elapsed_for_frame));
                                    if(sleepms > 0) {
                                        Sleep(sleepms);
                                    }

                                    test_seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter,
                                                                                               sglp_win32_get_wall_clock(),
                                                                                               perf_cnt_freq);
                                    if(test_seconds_elapsed_for_frame < target_seconds_per_frame) {
                                        // TODO(Jonny): Log missed Sleep here
                                    }

                                    while(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                        seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter,
                                                                                              sglp_win32_get_wall_clock(),
                                                                                              perf_cnt_freq);
                                    }
                                } else {
                                    // Missed Frame Rate!
                                }

                                {
                                    LARGE_INTEGER end_counter = sglp_win32_get_wall_clock();
                                    ms_per_frame = 1000.0f * win32_get_seconds_elapsed(last_counter, end_counter, perf_cnt_freq);
                                    last_counter = end_counter;
                                }

                                flip_wall_clock = sglp_win32_get_wall_clock();
                            }
                        }
                    }
                }
            }
        }
    }

    return(0);
}

#endif // SGLP_OS_WIN32

//
// Linux
//
#if SGLP_OS_LINUX

#if !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif

#include <X11/X.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <alsa/asoundlib.h>

#include <stdarg.h> // Var Args

#include <GL/glx.h>

#include <stdlib.h>

//
// X11
//
typedef Display *sglp_XOpenDisplay_t(int);
static sglp_XOpenDisplay_t *sglp_x11_XOpenDisplay;

typedef Colormap sglp_XCreateColorMap_t(Display *, Window, Visual *, int);
static sglp_XCreateColorMap_t *sglp_x11_XCreateColorMap;

typedef Window sglp_XCreateWindow_t(Display *, Window, int, int, unsigned int, unsigned int, int, int, unsigned int, Visual *, unsigned long, XSetWindowAttributes *);
static sglp_XCreateWindow_t *sglp_x11_XCreateWindow;

typedef void sglp_XMapWindow_t(Display *, Window);
static sglp_XMapWindow_t *sglp_x11_XMapWindow;

typedef void sglp_XStoreName_t(Display *, Window, char *);
static sglp_XStoreName_t *sglp_x11_XStoreName;

typedef void sglp_XSelectInput_t(Display *, Window, long);
static sglp_XSelectInput_t *sglp_x11_XSelectInput;

typedef int sglp_XPending_t(Display *);
static sglp_XPending_t *sglp_x11_XPending;

typedef void sglp_XNextEvent_t(Display *, XEvent *);
static sglp_XNextEvent_t *sglp_x11_XNextEvent;

typedef KeySym sglp_XKeycodeToKeysym_t(Display *, int, int);
static sglp_XKeycodeToKeysym_t *sglp_x11_XKeycodeToKeysym;

typedef void sglp_XGetWindowAttributes_t(Display *, Window, XWindowAttributes *);
static sglp_XGetWindowAttributes_t *sglp_x11_XGetWindowAttributes;

typedef int sglp_XQueryPointer_t(Display *, Window, Window *, Window *, int *, int *, int *, int *, unsigned int *);
static sglp_XQueryPointer_t *sglp_x11_XQueryPointer;

static sglp_Bool sglp_linux_load_x11(void) {
    sglp_Bool res = SGLP_FALSE;

    void *x11 = dlopen("libX11.so", RTLD_LAZY);
    if(x11) {
        sglp_x11_XOpenDisplay         = (sglp_XOpenDisplay_t *)        dlsym(x11, "XOpenDisplay");
        sglp_x11_XCreateColorMap      = (sglp_XCreateColorMap_t *)     dlsym(x11, "XCreateColormap");
        sglp_x11_XCreateWindow        = (sglp_XCreateWindow_t *)       dlsym(x11, "XCreateWindow");
        sglp_x11_XMapWindow           = (sglp_XMapWindow_t *)          dlsym(x11, "XMapWindow");
        sglp_x11_XStoreName           = (sglp_XStoreName_t *)          dlsym(x11, "XStoreName");
        sglp_x11_XSelectInput         = (sglp_XSelectInput_t *)        dlsym(x11, "XSelectInput");
        sglp_x11_XPending             = (sglp_XPending_t *)            dlsym(x11, "XPending");
        sglp_x11_XNextEvent           = (sglp_XNextEvent_t *)          dlsym(x11, "XNextEvent");
        sglp_x11_XKeycodeToKeysym     = (sglp_XKeycodeToKeysym_t *)    dlsym(x11, "XKeycodeToKeysym");
        sglp_x11_XGetWindowAttributes = (sglp_XGetWindowAttributes_t *)dlsym(x11, "XGetWindowAttributes");
        sglp_x11_XQueryPointer        = (sglp_XQueryPointer_t *)       dlsym(x11, "XQueryPointer");

        if((sglp_x11_XOpenDisplay) && (sglp_x11_XCreateColorMap) && (sglp_x11_XCreateWindow) && (sglp_x11_XMapWindow) && (sglp_x11_XStoreName) && (sglp_x11_XSelectInput) && (sglp_x11_XPending) && (sglp_x11_XNextEvent) && (sglp_x11_XKeycodeToKeysym) && (sglp_x11_XGetWindowAttributes)) {
            res = SGLP_TRUE;
        }
    }

    return(res);
}

//
// Posix Threads.
//
typedef sem_t *sglp_sem_open_t(const char *name, int oflag, mode_t mode, unsigned int value);
static sglp_sem_open_t *sglp_pthread_sem_open;

typedef int sglp_sem_close_t(sem_t *sem);
static sglp_sem_close_t *sglp_pthread_sem_close;

typedef int sglp_pthread_create_t(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
static sglp_pthread_create_t *sglp_pthread_pthread_create;

static sglp_Bool sglp_linux_load_lpthread(void) {
    sglp_Bool res = SGLP_FALSE;
    void *lpthread = 0;
    int32_t i = 0;

    char const *fallbacks[] = { "libpthread.so", "libpthread.so.0", "libpthread-2.19.so" };
    for(i = 0; (i < SGLP_ARRAY_COUNT(fallbacks)); ++i) {
        lpthread = dlopen(fallbacks[i], RTLD_LAZY);
        if(lpthread) {
            break; // for
        }
    }

    if(lpthread) {
        sglp_pthread_sem_open       = (sglp_sem_open_t *)      dlsym(lpthread, "sem_open");
        sglp_pthread_sem_close      = (sglp_sem_close_t *)     dlsym(lpthread, "sem_close");
        sglp_pthread_pthread_create = (sglp_pthread_create_t *)dlsym(lpthread, "pthread_create");

        if((sglp_pthread_sem_open) && (sglp_pthread_sem_close) && (sglp_pthread_pthread_create)) {
            res = SGLP_TRUE;
        }
    }

    return(res);
}

//
// OpenGL
//
typedef void *sglp_glXGetProcAddressArb_t(unsigned char *);
static sglp_glXGetProcAddressArb_t *sglp_gl_glXGetProcAddressARB;

typedef XVisualInfo *sglp_glXChooseVisual_t(Display *, int, sglp_GLint *);
static sglp_glXChooseVisual_t *sglp_gl_glXChooseVisual;

typedef GLXContext sglp_glXCreateContext_t(Display *, XVisualInfo *, int, int);
static sglp_glXCreateContext_t *sglp_gl_glXCreateContext;

typedef void sglp_glXMakeCurrent_t(Display *, Window, GLXContext);
static sglp_glXMakeCurrent_t *sglp_gl_glXMakeCurrent;

typedef void sglp_glXSwapBuffers_t(Display *, Window);
static sglp_glXSwapBuffers_t *sglp_gl_glXSwapBuffers;

static void *sglp_global_libGL = 0;

static sglp_Bool sglp_linux_load_gl(void) {
    sglp_Bool res = SGLP_FALSE;

    // TODO(Jonny): Have fallbacks incase libGL isn't the one they've got??
    sglp_global_libGL = dlopen("libGL.so", RTLD_LAZY);

    if(sglp_global_libGL) {
        sglp_gl_glXGetProcAddressARB = (sglp_glXGetProcAddressArb_t *)dlsym(sglp_global_libGL, "glXGetProcAddressARB");
        sglp_gl_glXChooseVisual      = (sglp_glXChooseVisual_t *)     dlsym(sglp_global_libGL, "glXChooseVisual");
        sglp_gl_glXCreateContext     = (sglp_glXCreateContext_t *)    dlsym(sglp_global_libGL, "glXCreateContext");
        sglp_gl_glXMakeCurrent       = (sglp_glXMakeCurrent_t *)      dlsym(sglp_global_libGL, "glXMakeCurrent");
        sglp_gl_glXSwapBuffers       = (sglp_glXSwapBuffers_t *)      dlsym(sglp_global_libGL, "glXSwapBuffers");

        if((sglp_gl_glXGetProcAddressARB) && (sglp_gl_glXChooseVisual) && (sglp_gl_glXCreateContext) && (sglp_gl_glXMakeCurrent) && (sglp_gl_glXSwapBuffers)) {
            res = SGLP_TRUE;
        }
    }

    return(res);
}

static sglp_Bool sglp_linux_load_opengl_funcs(void) {
    sglp_Bool res = SGLP_TRUE;

#define SGLP_LINUX_LOAD_OPENGL_FUNCTION(func_name) dlsym(sglp_global_libGL, func_name)

#define SGLP_OPENGL_ASSERT(OpenGLFunction) SGLP_ASSERT(OpenGLFunction); if(res) { res = (OpenGLFunction) != 0; }

    sglp_global_opengl->glBindTexture             = (sglp_glBindTexture_t *)            SGLP_LINUX_LOAD_OPENGL_FUNCTION("glBindTexture");             SGLP_OPENGL_ASSERT(sglp_global_opengl->glBindTexture);
    sglp_global_opengl->glClear                   = (sglp_glClear_t *)                  SGLP_LINUX_LOAD_OPENGL_FUNCTION("glClear");                   SGLP_OPENGL_ASSERT(sglp_global_opengl->glClear);
    sglp_global_opengl->glClearColor              = (sglp_glClearColor_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glClearColor");              SGLP_OPENGL_ASSERT(sglp_global_opengl->glClearColor);
    sglp_global_opengl->glDrawArrays              = (sglp_glDrawArrays_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glDrawArrays");              SGLP_OPENGL_ASSERT(sglp_global_opengl->glDrawArrays);
    sglp_global_opengl->glGetError                = (sglp_glGetError_t *)               SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGetError");                SGLP_OPENGL_ASSERT(sglp_global_opengl->glGetError);
    sglp_global_opengl->glTexImage2D              = (sglp_glTexImage2D_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glTexImage2D");              SGLP_OPENGL_ASSERT(sglp_global_opengl->glTexImage2D);
    sglp_global_opengl->glTexParameteri           = (sglp_glTexParameteri_t *)          SGLP_LINUX_LOAD_OPENGL_FUNCTION("glTexParameteri");           SGLP_OPENGL_ASSERT(sglp_global_opengl->glTexParameteri);
    sglp_global_opengl->glGetString               = (sglp_glGetString_t *)              SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGetString");               SGLP_OPENGL_ASSERT(sglp_global_opengl->glGetString);
    sglp_global_opengl->glViewport                = (sglp_glViewport_t *)               SGLP_LINUX_LOAD_OPENGL_FUNCTION("glViewport");                SGLP_OPENGL_ASSERT(sglp_global_opengl->glViewport);

    sglp_global_opengl->glCreateShader            = (sglp_glCreateShader_t *)           SGLP_LINUX_LOAD_OPENGL_FUNCTION("glCreateShader");            SGLP_OPENGL_ASSERT(sglp_global_opengl->glCreateShader);
    sglp_global_opengl->glShaderSource            = (sglp_glShaderSource_t *)           SGLP_LINUX_LOAD_OPENGL_FUNCTION("glShaderSource");            SGLP_OPENGL_ASSERT(sglp_global_opengl->glShaderSource);
    sglp_global_opengl->glCompileShader           = (sglp_glCompileShader_t *)          SGLP_LINUX_LOAD_OPENGL_FUNCTION("glCompileShader");           SGLP_OPENGL_ASSERT(sglp_global_opengl->glCompileShader);
    sglp_global_opengl->glGetShaderiv             = (sglp_glGetShaderiv_t *)            SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGetShaderiv");             SGLP_OPENGL_ASSERT(sglp_global_opengl->glGetShaderiv);
    sglp_global_opengl->glCreateProgram           = (sglp_glCreateProgram_t *)          SGLP_LINUX_LOAD_OPENGL_FUNCTION("glCreateProgram");           SGLP_OPENGL_ASSERT(sglp_global_opengl->glCreateProgram);
    sglp_global_opengl->glAttachShader            = (sglp_glAttachShader_t *)           SGLP_LINUX_LOAD_OPENGL_FUNCTION("glAttachShader");            SGLP_OPENGL_ASSERT(sglp_global_opengl->glAttachShader);
    sglp_global_opengl->glBindAttribLocation      = (sglp_glBindAttribLocation_t *)     SGLP_LINUX_LOAD_OPENGL_FUNCTION("glBindAttribLocation");      SGLP_OPENGL_ASSERT(sglp_global_opengl->glBindAttribLocation);
    sglp_global_opengl->glLinkProgram             = (sglp_glLinkProgram_t *)            SGLP_LINUX_LOAD_OPENGL_FUNCTION("glLinkProgram");             SGLP_OPENGL_ASSERT(sglp_global_opengl->glLinkProgram);
    sglp_global_opengl->glUseProgram              = (sglp_glUseProgram_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glUseProgram");              SGLP_OPENGL_ASSERT(sglp_global_opengl->glUseProgram);
    sglp_global_opengl->glGenBuffers              = (sglp_glGenBuffers_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGenBuffers");              SGLP_OPENGL_ASSERT(sglp_global_opengl->glGenBuffers);
    sglp_global_opengl->glBindBuffer              = (sglp_glBindBuffer_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glBindBuffer");              SGLP_OPENGL_ASSERT(sglp_global_opengl->glBindBuffer);
    sglp_global_opengl->glBufferData              = (sglp_glBufferData_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glBufferData");              SGLP_OPENGL_ASSERT(sglp_global_opengl->glBufferData);
    sglp_global_opengl->glVertexAttribPointer     = (sglp_glVertexAttribPointer_t *)    SGLP_LINUX_LOAD_OPENGL_FUNCTION("glVertexAttribPointer");     SGLP_OPENGL_ASSERT(sglp_global_opengl->glVertexAttribPointer);
    sglp_global_opengl->glEnableVertexAttribArray = (sglp_glEnableVertexAttribArray_t *)SGLP_LINUX_LOAD_OPENGL_FUNCTION("glEnableVertexAttribArray"); SGLP_OPENGL_ASSERT(sglp_global_opengl->glEnableVertexAttribArray);
    sglp_global_opengl->glUniform1i               = (sglp_glUniform1i_t *)              SGLP_LINUX_LOAD_OPENGL_FUNCTION("glUniform1i");               SGLP_OPENGL_ASSERT(sglp_global_opengl->glUniform1i);
    sglp_global_opengl->glUniform2f               = (sglp_glUniform2f_t *)              SGLP_LINUX_LOAD_OPENGL_FUNCTION("glUniform2f");               SGLP_OPENGL_ASSERT(sglp_global_opengl->glUniform2f);
    sglp_global_opengl->glUniform4f               = (sglp_glUniform4f_t *)              SGLP_LINUX_LOAD_OPENGL_FUNCTION("glUniform4f");               SGLP_OPENGL_ASSERT(sglp_global_opengl->glUniform4f);
    sglp_global_opengl->glUniformMatrix4fv        = (sglp_glUniformMatrix4fv_t *)       SGLP_LINUX_LOAD_OPENGL_FUNCTION("glUniformMatrix4fv");        SGLP_OPENGL_ASSERT(sglp_global_opengl->glUniformMatrix4fv);
    sglp_global_opengl->glGetUniformLocation      = (sglp_glGetUniformLocation_t *)     SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGetUniformLocation");      SGLP_OPENGL_ASSERT(sglp_global_opengl->glGetUniformLocation);
    sglp_global_opengl->glIsShader                = (sglp_glIsShader_t *)               SGLP_LINUX_LOAD_OPENGL_FUNCTION("glIsShader");                SGLP_OPENGL_ASSERT(sglp_global_opengl->glIsShader);
    sglp_global_opengl->glGetProgramiv            = (sglp_glGetProgramiv_t *)             SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGetProgramiv");            SGLP_OPENGL_ASSERT(sglp_global_opengl->glGetProgramiv);
    sglp_global_opengl->glGetProgramInfoLog       = (sglp_glGetProgramInfoLog_t *)      SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGetProgramInfoLog");       SGLP_OPENGL_ASSERT(sglp_global_opengl->glGetProgramInfoLog);
    sglp_global_opengl->glGetShaderInfoLog        = (sglp_glGetShaderInfoLog_t *)       SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGetShaderInfoLog");        SGLP_OPENGL_ASSERT(sglp_global_opengl->glGetShaderInfoLog);

    sglp_global_opengl->glGenVertexArrays         = (sglp_glGenVertexArrays_t *)        SGLP_LINUX_LOAD_OPENGL_FUNCTION("glGenVertexArrays");         SGLP_OPENGL_ASSERT(sglp_global_opengl->glGenVertexArrays);
    sglp_global_opengl->glBindVertexArray         = (sglp_glBindVertexArray_t *)        SGLP_LINUX_LOAD_OPENGL_FUNCTION("glBindVertexArray");         SGLP_OPENGL_ASSERT(sglp_global_opengl->glBindVertexArray);

#undef SGLP_LINUX_LOAD_OPENGL_FUNCTION
#undef SGLP_OPENGL_ASSERT

    return(res);
}

//
// Threading stuff
//
typedef struct WorkQueueEntry {
    void (*callback)(void *data);
    void *e;
} WorkQueueEntry;

typedef struct WorkQueue {
    int32_t volatile goal;
    int32_t volatile cnt;

    int32_t volatile next_entry_to_write;
    int32_t volatile next_entry_to_read;

    sem_t *hsem;
    WorkQueueEntry entries[256]; // TODO(Jonny): Should I make this a Linked List?? Or a dynamically resizing array??
} WorkQueue;
static WorkQueue sglp_global_work_queue;

static void sglp_linux_add_work_queue_entry(void *data, void (*callback)(void *data)) {
    int32_t new_next_entry_to_write = (sglp_global_work_queue.next_entry_to_write + 1) % SGLP_ARRAY_COUNT(sglp_global_work_queue.entries);
    WorkQueueEntry *entry = sglp_global_work_queue.entries + sglp_global_work_queue.next_entry_to_write;

    SGLP_ASSERT(new_next_entry_to_write != sglp_global_work_queue.next_entry_to_read);

    entry->callback = callback;
    entry->e = data;

    ++sglp_global_work_queue.goal;

    // TODO(Jonny): Put a full compiler/write barrier here!

    sglp_global_work_queue.next_entry_to_write = new_next_entry_to_write;
    if(sglp_pthread_sem_close) {
        sglp_pthread_sem_close(sglp_global_work_queue.hsem);
    }
}

static sglp_Bool sglp_linux_do_next_work_queue_entry(WorkQueue *work_queue) {
    sglp_Bool res = SGLP_FALSE;

    int32_t original_next_entry_to_read = work_queue->next_entry_to_read;
    int32_t new_next_entry_to_read = (original_next_entry_to_read + 1) % SGLP_ARRAY_COUNT(work_queue->entries);
    if(original_next_entry_to_read != work_queue->next_entry_to_write) {
        int32_t i = __sync_val_compare_and_swap(&work_queue->next_entry_to_read, original_next_entry_to_read, new_next_entry_to_read);
        if(i == original_next_entry_to_read) {
            WorkQueueEntry entry = work_queue->entries[i];
            entry.callback(entry.e);
            __sync_fetch_and_add(&work_queue->cnt, 1);
        }
    } else {
        res = SGLP_TRUE;
    }

    return(res);
}

static void sglp_linux_complete_all_work(void) {
    while(sglp_global_work_queue.goal != sglp_global_work_queue.cnt) {
        sglp_linux_do_next_work_queue_entry(&sglp_global_work_queue);
    }

    sglp_global_work_queue.goal = 0;
    sglp_global_work_queue.cnt = 0;
}

static void *sglp_linux_thread_proc(void *Parameter) {
    WorkQueue *queue = (WorkQueue *)Parameter;
    for(;;) {
        if(sglp_linux_do_next_work_queue_entry(queue)) {
            // TODO(Jonny): Find a function on Linux that will wait for the thread.
        }
    }
}

//
// sglp_File IO
//
static sglp_File sglp_linux_read_file(sglp_API *api, char const *fname) {
    sglp_File res = {0};

    int32_t file_desc = open(fname, O_RDONLY);
    if(file_desc != -1) {
        res.size = lseek(file_desc, 0, SEEK_END);
        lseek(file_desc, 0, SEEK_SET);

        res.e = (uint8_t *)api->os_malloc(res.size);
        read(file_desc, res.e, res.size);
    }

    return(res);
}

static sglp_File sglp_linux_read_file_and_null_terminate(sglp_API *api, char const *fname) {
    sglp_File res = {0};

    int32_t file_desc = open(fname, O_RDONLY);
    if(file_desc != -1) {
        res.size = lseek(file_desc, 0, SEEK_END);
        lseek(file_desc, 0, SEEK_SET);

        res.e = (uint8_t *)api->os_malloc(res.size + 1);
        read(file_desc, res.e, res.size);
    }

    return(res);
}

//
// Utility stuff
//
static uint64_t sglp_linux_get_processor_timestamp(void) {
    uint64_t res, low, high;

    __asm__("rdtsc" : "=a" (low), "=d" (high));

    SGLP_ASSERT((high < 0xFFFFFFFF) && (low < 0xFFFFFFFF));
    res = (high << 32) | low;

    return(res);
}

//
// Memory.
//
static void *sglp_linux_malloc(uintptr_t size) {
    void *res = malloc(size);
    if(res) {
        sglp_memset(res, 0, size);
    }

    return(res);
}

static void *sglp_linux_realloc(void *ptr, uintptr_t size) {
    void *res = realloc(ptr, size);

    return(res);
}

static void sglp_linux_free(void *ptr) {
    if(ptr) {
        free(ptr);
    }
}

int main(int argc, char **argv) {
    sglp_API api = {0};

    api.free_file = sglp_free_file;
    api.read_file = sglp_linux_read_file;
    api.read_file_and_null_terminate = sglp_linux_read_file_and_null_terminate;

    api.get_processor_timestamp = sglp_linux_get_processor_timestamp;

    api.add_work_queue_entry = sglp_linux_add_work_queue_entry;
    api.complete_all_work = sglp_linux_complete_all_work;

    api.os_malloc = sglp_linux_malloc;
    api.os_realloc = sglp_linux_realloc;
    api.os_free = sglp_linux_free;

    sglp_global_opengl = &api.gl;

    if((sglp_linux_load_x11()) && (sglp_linux_load_lpthread()) && (sglp_linux_load_gl())) {
        Display *disp;

        // TODO(Jonny): Read from hardware!
        api.settings.frame_rate = 60;
        api.settings.thread_cnt = 8;

        api.settings.win_width = 1920 / 2;
        api.settings.win_height = 1080 / 2;

        sglp_platform_setup_settings_callback(&api.settings);

        disp = sglp_x11_XOpenDisplay(0);
        if(disp) {
            sglp_GLint attrib_list[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
            XVisualInfo *visual_info = sglp_gl_glXChooseVisual(disp, 0, attrib_list);
            if(visual_info) {
                Window root_window = DefaultRootWindow(disp);
                Colormap colour_map = sglp_x11_XCreateColorMap(disp, root_window, visual_info->visual, AllocNone);

                XSetWindowAttributes set_window_attributes = {0};
                set_window_attributes.colormap = colour_map;
                set_window_attributes.event_mask = ExposureMask | KeyPressMask;

                if(!api.settings.win_width) {
                    api.settings.win_width = 1920 / 2;
                }

                if(!api.settings.win_height) {
                    api.settings.win_height = 1080 / 2;
                }

                Window win = sglp_x11_XCreateWindow(disp, root_window, 0, 0, api.settings.win_width,
                                                    api.settings.win_height, 0, visual_info->depth, InputOutput,
                                                    visual_info->visual, CWColormap | CWEventMask, &set_window_attributes);
                if(win) {
                    sglp_x11_XMapWindow(disp, win);
                    sglp_x11_XStoreName(disp, win, (char *)api.settings.window_title);

                    GLXContext gl_context = sglp_gl_glXCreateContext(disp, visual_info, 0, GL_TRUE);
                    sglp_x11_XSelectInput(disp, win, KeyPressMask | KeyReleaseMask);
                    if(gl_context) {
                        sglp_gl_glXMakeCurrent(disp, win, gl_context);

                        api.permanent_memory = api.os_malloc(api.settings.permanent_memory_size);
                        if(api.permanent_memory) {
                            WorkQueue work_queue = {0};

                            if(api.settings.thread_cnt > 0) {
                                if(sglp_pthread_sem_open) {
                                    work_queue.hsem = sglp_pthread_sem_open("Thread", O_CREAT | O_EXCL, 0644, 1);
                                }

                                for(int i = 0; (i < api.settings.thread_cnt); ++i) {
                                    pthread_t Thread;
                                    if(sglp_pthread_pthread_create) {
                                        sglp_pthread_pthread_create(&Thread, 0, sglp_linux_thread_proc, (void *)&work_queue);
                                    }
                                }
                            }

                            if(sglp_linux_load_opengl_funcs()) {
                                int channels = 2;
                                int samples_per_second = 48000;
                                int sample_rate = samples_per_second;
                                int bytes_per_sample = sizeof(int16_t) * 2;

                                // Open the PCM device in playback mode.
                                snd_pcm_t *pcm_handle = 0;
                                snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

                                // Allocate parameters object and fill it with default values.
                                snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)sglp_linux_malloc(snd_pcm_hw_params_sizeof());
                                snd_pcm_hw_params_any(pcm_handle, params);

                                // Set parameters
                                snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
                                snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
                                snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
                                snd_pcm_hw_params_set_rate(pcm_handle, params, sample_rate, 0);

                                // Write params.
                                snd_pcm_hw_params(pcm_handle, params);

                                // Allocate buffer to hold single period.
                                snd_pcm_uframes_t frames;
                                int dir;
                                snd_pcm_hw_params_get_period_size(params, &frames, &dir);

                                int16_t *snd_samples =
                                    (int16_t *)sglp_linux_malloc((samples_per_second * bytes_per_sample) + ((2 * 8) * sizeof(int16_t)));

                                sglp_setup(&api, api.settings.max_no_of_sounds);

                                XWindowAttributes win_attribs = {0};
                                sglp_Bool quit = SGLP_FALSE; // TODO(Jonny): Should quit be in sglp_API?

                                int32_t last_time = 0;
                                int32_t this_time = 0;
                                float target_ms_per_frame = 16.666666f;
                                float actual_ms_per_frame = target_ms_per_frame;

                                api.init_game = SGLP_TRUE;
                                api.dt = 16.6f; // TODO(Jonny): Hacky!

                                while(!quit) {
                                    while(sglp_x11_XPending(disp)) {
                                        XEvent event;
                                        sglp_x11_XNextEvent(disp, &event);
                                        if(KeyPress == event.xkey.type) {
                                            KeySym X11Key = sglp_x11_XKeycodeToKeysym(disp, event.xkey.keycode, 0);
                                            switch(X11Key) {
                                                case XK_Up:    { api.key[sglp_key_up]    = SGLP_TRUE; } break;
                                                case XK_Down:  { api.key[sglp_key_down]  = SGLP_TRUE; } break;
                                                case XK_Left:  { api.key[sglp_key_left]  = SGLP_TRUE; } break;
                                                case XK_Right: { api.key[sglp_key_right] = SGLP_TRUE; } break;

                                                case 'w': { api.key[sglp_key_w] = SGLP_TRUE; } break;
                                                case 's': { api.key[sglp_key_s] = SGLP_TRUE; } break;
                                                case 'a': { api.key[sglp_key_a] = SGLP_TRUE; } break;
                                                case 'd': { api.key[sglp_key_d] = SGLP_TRUE; } break;

                                                case XK_space: { api.key[sglp_key_space] = SGLP_TRUE; } break;

                                                case XK_Escape: { api.key[sglp_key_escape] = SGLP_TRUE; } break;
                                            }
                                        } else if(KeyRelease == event.xkey.type) {
                                            KeySym X11Key = sglp_x11_XKeycodeToKeysym(disp, event.xkey.keycode, 0);
                                            switch(X11Key) {
                                                case XK_Up:    { api.key[sglp_key_up]    = SGLP_FALSE; } break;
                                                case XK_Down:  { api.key[sglp_key_down]  = SGLP_FALSE; } break;
                                                case XK_Left:  { api.key[sglp_key_left]  = SGLP_FALSE; } break;
                                                case XK_Right: { api.key[sglp_key_right] = SGLP_FALSE; } break;

                                                case 'w': { api.key[sglp_key_w] = SGLP_FALSE; } break;
                                                case 's': { api.key[sglp_key_s] = SGLP_FALSE; } break;
                                                case 'a': { api.key[sglp_key_a] = SGLP_FALSE; } break;
                                                case 'd': { api.key[sglp_key_d] = SGLP_FALSE; } break;

                                                case XK_space: { api.key[sglp_key_space] = SGLP_FALSE; } break;

                                                case XK_Escape: { api.key[sglp_key_escape] = SGLP_FALSE; } break;
                                            }
                                        }
                                    }

                                    sglp_x11_XGetWindowAttributes(disp, win, &win_attribs);

                                    sglp_platform_update_and_render_callback(&api);
                                    api.init_game = SGLP_FALSE;

                                    sglp_gl_glXSwapBuffers(disp, win);

                                    sglp_SoundOutputBuffer sound_buffer = {0};
                                    sound_buffer.samples_per_second = samples_per_second;
                                    sound_buffer.sample_cnt = SGLP_ALIGN8((int)(samples_per_second / actual_ms_per_frame));
                                    sound_buffer.samples = snd_samples;

                                    sglp_output_playing_sounds(&api, &sound_buffer);

                                    int pcmrc = snd_pcm_writei(pcm_handle, sound_buffer.samples, sound_buffer.sample_cnt);
                                    if(pcmrc == -EPIPE) {
                                        fprintf(stderr, "Oops\n");
                                        snd_pcm_prepare(pcm_handle);
                                    }

                                    // TODO(Jonny): The frame rate stuff isn't working 100%...
#if 0
                                    struct timespec TimeSpec;
                                    clock_gettime(0, &TimeSpec);
                                    last_time = this_time;
                                    this_time = TimeSpec.tv_nsec;

                                    float SecondsElapsedForFrame = cast(float)(this_time - last_time) / 1000000.0f;
                                    actual_ms_per_frame = SecondsElapsedForFrame;
                                    if(actual_ms_per_frame < 0) {
                                        actual_ms_per_frame = target_ms_per_frame;
                                    }

                                    int TimeToSleep = (target_ms_per_frame - SecondsElapsedForFrame) * 1000;
                                    if(TimeToSleep > 0 && TimeToSleep < target_ms_per_frame) {
                                        usleep(TimeToSleep);
                                    }
#endif
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return(0);
}

#endif // SGLP_OS_LINUX

#endif // SGLP_USE_SDL

#endif // #if defined(SGLP_IMPLEMENTATION)

#define _SGLP_PLATFORM_H
#endif

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - MIT License
Copyright (c) 2017 Jonathan Livingstone
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
*/