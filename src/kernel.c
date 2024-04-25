#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../headers/limine.h"

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

static volatile LIMINE_BASE_REVISION(1);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once.

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}
/*
char ascii_pattern[] =
" ###   #  ###  #  ### ##  ###  ##"
" # #  # #  #  # #  #  #   # # #  "
" ###  ###  #  ###  #  ##  # #  # "
" #    # #  #  # #  #  #   # #   #"
" #    # #  #  # #  #  ##  ### ## ";
*/

char ascii_pattern[] = 
" ###   #  ###  #  ### ##  ###   ##"
" # #  # #  #  # #  #  #   # #  #  "
" ###  ###  #  ###  #  ##  # #   # "
" #    # #  #  # #  #  #   # #    #"
" #    # #  #  # #  #  ##  ###  ## ";

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
void _start(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
            || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    size_t n = 2; //size of cubes
    size_t width = 34;
    size_t height = 5;
    size_t hori_offset = framebuffer->width/2 - width * n ;
    size_t vert_offset = (framebuffer->height/2 -height *n )* framebuffer->width;
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            // Calculer l'indice dans le motif ASCII correspondant au pixel actuel
            size_t ascii_index = y * width + x;

            // Récupérer le caractère ASCII correspondant
            char pixel_char = ascii_pattern[ascii_index];

            // Convertir le caractère ASCII en couleur (par exemple, blanc)
            uint32_t pixel_color = 0x0Affff;

            // Vérifier si le pixel est un caractère représentant un espace ou un motif
            if (pixel_char == ' ' ) {
                pixel_color = 0x000000; // Noir pour les espaces
            }

            volatile uint32_t *fb_ptr = framebuffer->address;
            fb_ptr[vert_offset + hori_offset + y*4 * (framebuffer->pitch / 4) + x*4] = pixel_color;
            fb_ptr[vert_offset + hori_offset + y*4   * (framebuffer->pitch / 4) + x*4+1] = pixel_color;
          fb_ptr[vert_offset + hori_offset + (y+1)*4 * (framebuffer->pitch / 4) + x*4  ] = pixel_color;
            fb_ptr[vert_offset + hori_offset + (y+1)*4 * (framebuffer->pitch / 4) + x*4+1] = pixel_color; */
        }
    }
    // We're done, just hang...
    hcf();
}
