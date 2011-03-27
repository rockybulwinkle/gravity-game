/*
 * bloom.h
 *
 *  Created on: Mar 26, 2011
 *      Author: hopwoocp
 */

#ifndef BLOOM_H_
#define BLOOM_H_

#endif /* BLOOM_H_ */
// Storage to capture the rendered scene
GRRLIB_texImg *tex_screen;

// Storage for the bloom texture
GRRLIB_texImg *tex_bloom;

// You can tweak these next five constants...
#define BLOOM_WIDTH_SCALE 2
#define BLOOM_HEIGHT_SCALE 2
#define BLOOM_RADIUS 40 // Works best as an odd number
#define BLOOM_THRESHOLD 100 // 0 -> 255
#define BLOOM_ALPHA 0x99

// ...But you should leave these alone!
#define BLUR_STACK_LEFT ( ( BLOOM_RADIUS + 1 ) / 2 )
#define BLUR_STACK_RIGHT ( ( BLOOM_RADIUS - 1 ) / 2 )
#define BLOOM_TEX_WIDTH ( 640 / BLOOM_WIDTH_SCALE )
#define BLOOM_TEX_HEIGHT ( 480 / BLOOM_HEIGHT_SCALE )
#define BLOOM_STACK_LOOKUP_SIZE ( ( UINT8_MAX * BLOOM_RADIUS ) + 1 )
guVector screen_quad[4] = { { 0, 0, 0 }, { 640, 0, 0 }, { 640, 480, 0 }, { 0,
		480, 0 } };
int bloom = 1;
static u8 avg_table[BLOOM_STACK_LOOKUP_SIZE];
struct comp_s {
	u8 r, g, b, a;
}__attribute__ ((packed));

typedef union {
	struct comp_s comp;
	u32 rgba;
} rgbaPixel_u;

static rgbaPixel_u temp[BLOOM_TEX_WIDTH * BLOOM_TEX_HEIGHT]; // Stack blur temp buffer

// Modified version of GRRLIB_GetPixelFromtexImg(). Returns 0 if the grayscale value
// of the pixel is less than BLOOM_THRESHOLD.
static inline u32 _GetPixel(const int x, const int y, const GRRLIB_texImg *tex) {
	register u32 offs;
	register u32 ar;
	register u8* bp = (u8*) tex->data;

	offs = (((y & (~3)) << 2) * tex->w) + ((x & (~3)) << 4) + ((((y & 3) << 2)
			+ (x & 3)) << 1);
	ar = (u32) (*((u16*) (bp + offs)));
	rgbaPixel_u
			pixel = {
					.rgba = (ar<<24) | ( ((u32)(*((u16*)(bp+offs+32)))) <<8) | (ar>>8) };

	u32 gray = ((pixel.comp.r * 11) + (pixel.comp.g * 16) + (pixel.comp.b * 5))
			/ 32;
	if (gray > BLOOM_THRESHOLD) {
		return pixel.rgba;
	}
	return 0;
}

// initialize the bloom stuff

void initializeBloom() {
	tex_bloom = GRRLIB_CreateEmptyTexture(BLOOM_TEX_WIDTH, BLOOM_TEX_HEIGHT );
	tex_screen = GRRLIB_CreateEmptyTexture(rmode->fbWidth, rmode->efbHeight);
	u32 i;
	for (i = 0; i < BLOOM_STACK_LOOKUP_SIZE; ++i) {
		avg_table[i] = (u8) ((i / (float) BLOOM_RADIUS) + 0.5);
	}
}

void doBloom() {
	if (bloom == true) {
		// Capture the rendered scene to a texture
		GRRLIB_Screen2Texture(0, 0, tex_screen, false);

		u32 w, h, r, g, b, i;
		rgbaPixel_u read, write;

		// Horizontal scale and blur each row into the temp buffer
		for (h = 0; h < BLOOM_TEX_HEIGHT; ++h) {
			// Build the initial stack and calculate the first pixel
			read.rgba = _GetPixel((0 * BLOOM_HEIGHT_SCALE) + 1, (h
					* BLOOM_WIDTH_SCALE) + 1, tex_screen);
			r = read.comp.r * BLUR_STACK_LEFT;
			g = read.comp.g * BLUR_STACK_LEFT;
			;
			b = read.comp.b * BLUR_STACK_LEFT;
			;

			for (i = 1; i < BLUR_STACK_RIGHT + 1; ++i) {
				read.rgba = _GetPixel((i * BLOOM_HEIGHT_SCALE) + 1, (h
						* BLOOM_WIDTH_SCALE) + 1, tex_screen);
				r += read.comp.r;
				g += read.comp.g;
				b += read.comp.b;
			}

			write.comp.r = avg_table[r];
			write.comp.g = avg_table[g];
			write.comp.b = avg_table[b];
			write.comp.a = BLOOM_ALPHA;
			temp[(h * BLOOM_TEX_WIDTH) + 0] = write;

			for (w = 1; w < BLOOM_TEX_WIDTH; ++w) {
				// Pop the 'left' pixel from the stack
				if (w >= BLUR_STACK_LEFT) {
					read.rgba = _GetPixel(((w - BLUR_STACK_LEFT)
							* BLOOM_HEIGHT_SCALE) + 1, (h * BLOOM_WIDTH_SCALE)
							+ 1, tex_screen);
				} else {
					read.rgba = _GetPixel((0 * BLOOM_HEIGHT_SCALE) + 1, (h
							* BLOOM_WIDTH_SCALE) + 1, tex_screen);
				}
				r -= read.comp.r;
				g -= read.comp.g;
				b -= read.comp.b;

				// Push the 'right' pixel on to the stack
				if (w <= BLOOM_TEX_WIDTH - 1 - BLUR_STACK_RIGHT) {
					read.rgba = _GetPixel(((w + BLUR_STACK_RIGHT)
							* BLOOM_HEIGHT_SCALE) + 1, (h * BLOOM_WIDTH_SCALE)
							+ 1, tex_screen);
				} else {
					read.rgba = _GetPixel(BLOOM_TEX_WIDTH, (h
							* BLOOM_WIDTH_SCALE) + 1, tex_screen);
				}
				r += read.comp.r;
				g += read.comp.g;
				b += read.comp.b;

				write.comp.r = avg_table[r];
				write.comp.g = avg_table[g];
				write.comp.b = avg_table[b];
				write.comp.a = BLOOM_ALPHA;
				temp[(h * BLOOM_TEX_WIDTH) + w] = write;
			}
		}

		// Vertical blur the temp buffer to the bloom texture
		for (w = 0; w < BLOOM_TEX_WIDTH; ++w) {
			// Build the initial stack and calculate the first pixel;
			read = temp[(0 * BLOOM_TEX_WIDTH) + w];
			r = read.comp.r * BLUR_STACK_LEFT;
			g = read.comp.g * BLUR_STACK_LEFT;
			b = read.comp.b * BLUR_STACK_LEFT;

			for (i = 1; i < BLUR_STACK_RIGHT + 1; ++i) {
				read = temp[(i * BLOOM_TEX_WIDTH) + w];
				r += read.comp.r;
				g += read.comp.g;
				b += read.comp.b;
			}

			write.comp.r = avg_table[r];
			write.comp.g = avg_table[g];
			write.comp.b = avg_table[b];
			write.comp.a = BLOOM_ALPHA;
			GRRLIB_SetPixelTotexImg(w + 1, 1, tex_bloom, write.rgba);

			for (h = 1; h < BLOOM_TEX_HEIGHT; ++h) {
				// Pop the 'top' pixel from the stack
				if (h >= BLUR_STACK_LEFT) {
					read = temp[((h - BLUR_STACK_LEFT) * BLOOM_TEX_WIDTH) + w];
				} else {
					read = temp[(0 * BLOOM_TEX_WIDTH) + w];
				}
				r -= read.comp.r;
				g -= read.comp.g;
				b -= read.comp.b;

				// Push the 'bottom' pixel on to the stack
				if (h <= BLOOM_TEX_HEIGHT - 1 - BLUR_STACK_RIGHT) {
					read = temp[((h + BLUR_STACK_RIGHT) * BLOOM_TEX_WIDTH) + w];
				} else {
					read = temp[((h - BLUR_STACK_LEFT) * BLOOM_TEX_WIDTH) + w];
				}
				r += read.comp.r;
				g += read.comp.g;
				b += read.comp.b;

				write.comp.r = avg_table[r];
				write.comp.g = avg_table[g];
				write.comp.b = avg_table[b];
				write.comp.a = BLOOM_ALPHA;
				GRRLIB_SetPixelTotexImg(w + 1, h + 1, tex_bloom, write.rgba);
			}
		}

		GRRLIB_FlushTex(tex_bloom);
		GRRLIB_SetBlend(GRRLIB_BLEND_ADD);
		GRRLIB_DrawImgQuad(screen_quad, tex_bloom, 0xFFFFFFFF);
		GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);
	}
}
