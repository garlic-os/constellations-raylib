/**
 * Facilitates interaction with HTML5 Canvas elements in a similar
 * manner to JavaScript via the DOM, but from C to be compiled with Emscripten.
 * @brief HTMLCanvasElement and CanvasRenderingContext2D C-DOM-JS-interaction
 * @file canvas.hpp
 * @author Alex Tyner
 * 
 * Modified to follow C++ conventions by garlicOS®
 */
#ifndef CANVAS_H
#define CANVAS_H

#include <emscripten.h>
#include <string.h>
#include <stdlib.h>

typedef struct HTMLCanvasElement HTMLCanvasElement;
typedef struct CanvasRenderingContext2D CanvasRenderingContext2D;

/**
 * Struct containing state and OO-like behavior of a CanvasRenderingContext2D structured similarly
 * to how it would be exposed in JavaScript. This struct should not be instantiated, but rather 
 * obtained from an HTMLCanvas struct by calling its getContext() function pointer.
 * 
 * Function pointers in this struct can be called in an almost identical manner to their JavaScript
 * equivalents, with a few idiomatic-C quirks. Firstly, every function takes as its first parameter
 * a pointer to the struct itself. It's hard to access the struct's state within the function otherwise
 * since we are just simulating OO. Secondly, some function parameters differ slightly from the JS
 * implementations because we don't have overloading in C. For example, the fillText() function
 * always expects the optional 'maxWidth' property. That parameter will be ignored if you provide a
 * negative value, and there are comments written below with that and similar information.
 * 
 * You might notice that a lot of functions expect double parameters. That's because most of the
 * equivalent JavaScript functions will accept doubles. Don't worry, providing int parameters will
 * work just fine. (int) -> (double) is an implicit conversion in C; no cast required.
 * 
 * To summarize, a typical use of this struct might look like the following:
 * 
 *     HTMLCanvas *canvas = createCanvas("myCanvas");
 *     CanvasRenderingContext2D *ctx = canvas->getContext(canvas, "2d"); // only 2d is supported currently
 *     ctx->setFillStyle(ctx, "#FF0000");
 *     printf("I set the fill style to %s\n", ctx->getFillStyle(ctx));
 *     ctx->fillRect(ctx, 50, 75, 100, 200);
 *     freeCanvas(canvas);
 * 
 * Some state is pseudo-encapsulated in the 'private' member struct, such as pointers to dynamically
 * allocated strings that will need to be freed when the HTMLCanvas is freed. Typically these
 * are from JavaScript calls, allocated to return them to the user. Alternatively, functions
 * with canonical string return types could be pigeon-holed into returning enum types, or
 * we could make the user responsible for buffer allocation and expect an additional by-reference
 * parameter to copy the string into. The in-JavaScript allocated string would need to be
 * freed after returning its contents to the user, and the user would not know how large the buffer
 * should be. So, when a string is exposed to the user, this struct keeps track of the pointers in
 * order to free them when the HTMLCanvas parent struct is freed.
 */
struct CanvasRenderingContext2D
{
    /**
     * This anonymous struct encapsulates fields of the CanvasRenderingContext2D struct
     * intended to be private. These are primarily pointers to memory allocated from
     * JavaScript calls so that they can be freed when the canvas is no longer in use.
     */
    struct
    {
        HTMLCanvasElement *canvas;
        char contextType[19];
        char *font;
        char *textAlign;
        char *fillStyle;
        char *strokeStyle;
        char *lineCap;
        char *lineJoin;
        char *globalCompositeOperation;
    } privado;
    void (*clearRect)(CanvasRenderingContext2D *that, double x, double y, double width, double height);
    void (*fillRect)(CanvasRenderingContext2D *that, double x, double y, double width, double height);
    void (*strokeRect)(CanvasRenderingContext2D *that, double x, double y, double width, double height);
    /** @param maxWidth optional parameter. provide a value < 0.0 to ignore this parameter. */
    void (*fillText)(CanvasRenderingContext2D *that, char const *text, double x, double y, double maxWidth);
    /** @param maxWidth optional parameter. provide a value < 0.0 to ignore this parameter. */
    void (*strokeText)(CanvasRenderingContext2D *that, char const *text, double x, double y, double maxWidth);
    /* TextMetrics *(*measureText)(CanvasRenderingContext2D *that, char const *text); */ // that's a whole can of worms
    void (*setLineWidth)(CanvasRenderingContext2D *that, double value);
    double (*getLineWidth)(CanvasRenderingContext2D *that);
    void (*setLineCap)(CanvasRenderingContext2D *that, char const *type);
    char const *(*getLineCap)(CanvasRenderingContext2D *that);
    void (*setLineJoin)(CanvasRenderingContext2D *that, char const *type);
    char const *(*getLineJoin)(CanvasRenderingContext2D *that);
    char const *(*getFont)(CanvasRenderingContext2D *that);
    void (*setFont)(CanvasRenderingContext2D *that, char const *value);
    void (*setTextAlign)(CanvasRenderingContext2D *that, char const *value);
    char const *(*getTextAlign)(CanvasRenderingContext2D *that);
    void (*setFillStyle)(CanvasRenderingContext2D *that, char const *value);
    char const *(*getFillStyle)(CanvasRenderingContext2D *that);
    void (*setStrokeStyle)(CanvasRenderingContext2D *that, char const *value);
    char const *(*getStrokeStyle)(CanvasRenderingContext2D *that);
    void (*beginPath)(CanvasRenderingContext2D *that);
    void (*closePath)(CanvasRenderingContext2D *that);
    void (*moveTo)(CanvasRenderingContext2D *that, double x, double y);
    void (*lineTo)(CanvasRenderingContext2D *that, double x, double y);
    void (*bezierCurveTo)(CanvasRenderingContext2D *that, double cp1x, double cp1y, double cp2x, double cp2y, double x, double y);
    void (*quadraticCurveTo)(CanvasRenderingContext2D *that, double cpx, double cpy, double x, double y);
    void (*arc)(CanvasRenderingContext2D *that, double x, double y, double radius, double startAngle, double endAngle);
    void (*arcTo)(CanvasRenderingContext2D *that, double x1, double y1, double x2, double y2, double radius);
    void (*ellipse)(CanvasRenderingContext2D *that, double x, double y, double radiusX, double radiusY, double rotation, double startAngle, double endAngle);
    void (*rect)(CanvasRenderingContext2D *that, double x, double y, double width, double height);
    void (*fill)(CanvasRenderingContext2D *that);
    void (*stroke)(CanvasRenderingContext2D *that);
    void (*clip)(CanvasRenderingContext2D *that);
    int (*isPointInPath)(CanvasRenderingContext2D *that, double x, double y);
    int (*isPointInStroke)(CanvasRenderingContext2D *that, double x, double y);
    void (*rotate)(CanvasRenderingContext2D *that, double angle);
    void (*scale)(CanvasRenderingContext2D *that, double x, double y);
    void (*translate)(CanvasRenderingContext2D *that, double x, double y);
    void (*transform)(CanvasRenderingContext2D *that, double a, double b, double c, double d, double e, double f);
    void (*setTransform)(CanvasRenderingContext2D *that, double a, double b, double c, double d, double e, double f);
    void (*resetTransform)(CanvasRenderingContext2D *that);
    void (*setGlobalAlpha)(CanvasRenderingContext2D *that, double value);
    double (*getGlobalAlpha)(CanvasRenderingContext2D *that);
    void (*setGlobalCompositeOperation)(CanvasRenderingContext2D *that, char const *value);
    char const *(*getGlobalCompositeOperation)(CanvasRenderingContext2D *that);
    void (*save)(CanvasRenderingContext2D *that);
    void (*restore)(CanvasRenderingContext2D *that);
    HTMLCanvasElement *(*getCanvas)(CanvasRenderingContext2D *that);
};

/**
 * Struct containing state and OO-like behavior of an HTML canvas structured similarly
 * to how it would be exposed in JavaScript. This struct should be instantiated using
 * the createCanvas() function, and, when you're done using it, should be freed using the
 * freeCanvas() function.
 * 
 * After freeing the canvas struct, the DOM element will still be present and active in
 * HTML. In the future, it may be possible to reacquire the same canvas as a struct by calling
 * createCanvas() with the same element id.
 * 
 * The real meat of this struct comes with the getContext() function pointer, which creates
 * a canvas rendering context with drawing capabilities.
 * 
 * A typical use of this struct might look like the following:
 * 
 *     HTMLCanvas *canvas = createCanvas("myCanvas");
 *     canvas->setHeight(canvas, 1080);
 *     canvas->setWidth(canvas, 1920);
 *     CanvasRenderingContext2D *ctx = canvas->getContext(canvas, "2d"); // only 2d is supported currently
 *     ctx->fillRect(ctx, 50, 75, 100, 200);
 *     freeCanvas(canvas);
 */
struct HTMLCanvasElement
{
    struct
    {
        CanvasRenderingContext2D *ctx;
        char *id;
    } privado;
    /** 
     * Returns a positive integer reflecting the height HTML attribute of the <canvas> element
     * interpreted in CSS pixels. The canvas height defaults to 150. 
     */
    int (*getHeight)(HTMLCanvasElement *that);
    /** 
     * Returns a positive integer reflecting the width HTML attribute of the <canvas> element
     * interpreted in CSS pixels. The canvas width defaults to 300. 
     */
    int (*getWidth)(HTMLCanvasElement *that);
    /**
     * Sets the height HTML attribute of the <canvas> element. If an invalid value is specified,
     * the default value of 150 is used. 
     */
    void (*setHeight)(HTMLCanvasElement *that, int height);
    /**
     * Sets the width HTML attribute of the <canvas> element. If an invalid value is specified,
     * the default value of 300 is used. 
     */
    void (*setWidth)(HTMLCanvasElement *that, int width);
    /** 
     * Returns a drawing context for the canvas, or null if the context type is not supported.
     * Use type "2d" (only this type is currently supported) to retrieve a CanvasRenderingContext2D.
     * 
     * The field retrieved by this getter function behaves like a singleton. 
     */
    CanvasRenderingContext2D *(*getContext)(HTMLCanvasElement *that, char const *contextType);
};

/**
 * Creates a struct containing state and OO-like behavior of an HTML canvas structured
 * similarly to how it would be exposed in JavaScript. This struct should be instantiated using
 * the createCanvas() function, and, when you're done using it, should be freed using the
 * freeCanvas() function.
 * 
 * After freeing the canvas struct, the DOM element will still be present and active in
 * HTML. It is possible to acquire an existing HTML canvas or reacquire a previously freed
 * canvas by calling createCanvas() with its matching element id.
 * 
 * A typical use of this function might look like the following:
 * 
 *     HTMLCanvas *canvas = createCanvas("myCanvas");
 *     canvas->setHeight(canvas, 1080);
 *     canvas->setWidth(canvas, 1920);
 *     CanvasRenderingContext2D *ctx = canvas->getContext(canvas, "2d"); // only 2d is supported currently
 *     ctx->fillRect(ctx, 50, 75, 100, 200);
 *     freeCanvas(canvas);
 *     // I've decided I want that canvas again
 *     HTMLCanvas *sameOldCanvas = createCanvas("myCanvas");
 *     int width = sameOldCanvas->getWidth(sameOldCanvas);
 *     freeCanvas(sameOldCanvas);
 */
HTMLCanvasElement *createCanvas(char const *name);

/**
 * Frees the dynamically allocated HTMLCanvasElement and any dynamically allocated
 * state as necessary. The DOM canvas element will still exist in HTML after freeing
 * the struct.
 */
void freeCanvas(HTMLCanvasElement *canvas);

#endif
