/**
 * Facilitates interaction with HTML5 Canvas elements in a similar
 * manner to JavaScript via the DOM, but from C to be compiled with Emscripten.
 * @file canvas.cpp
 * @author Alex Tyner
 * 
 * Modified to follow C++ conventions by garlicOS®
 */

#include "canvas.h"

static CanvasRenderingContext2D *createContext(HTMLCanvasElement *canvas, char const *contextType);

/* Begin: HTMLCanvasElement static methods */
static int canvas_getWidth(HTMLCanvasElement *that)
{
    return EM_ASM_INT({
        return document.getElementById(UTF8ToString($0)).width;
    },
                      that->privado.id);
}
static int canvas_getHeight(HTMLCanvasElement *that)
{
    return EM_ASM_INT({
        return document.getElementById(UTF8ToString($0)).height;
    },
                      that->privado.id);
}
static void canvas_setWidth(HTMLCanvasElement *that, int width)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).width = $1;
    },
           that->privado.id, width);
}
static void canvas_setHeight(HTMLCanvasElement *that, int height)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).height = $1;
    },
           that->privado.id, height);
}
static CanvasRenderingContext2D *canvas_getContext(HTMLCanvasElement *that, char const *contextType)
{
    if (!that->privado.ctx)
        that->privado.ctx = createContext(that, contextType);
    return that->privado.ctx;
}
/* End: HTMLCanvasElement static methods */

HTMLCanvasElement *createCanvas(char const *id)
{
    EM_ASM(
        {
            if (!document.getElementById(UTF8ToString($0)))
                document.body.appendChild(document.createElement("canvas")).setAttribute("id", UTF8ToString($0));
        },
        id);
    HTMLCanvasElement *c = (HTMLCanvasElement *)malloc(sizeof(HTMLCanvasElement));
    /* Begin: set pseudo-privado fields */
    c->privado.id = (char *)malloc(strlen(id) + 1);
    strcpy(c->privado.id, id);
    c->privado.ctx = NULL; // we'll lazy-load the context when it's asked for
    /* End: set pseudo-privado fields */
    c->getWidth = canvas_getWidth;
    c->getHeight = canvas_getHeight;
    c->setHeight = canvas_setHeight;
    c->setWidth = canvas_setWidth;
    c->getContext = canvas_getContext;
    return c;
}

/* Begin: CanvasRenderingContext2D static methods */
static void context2d_clearRect(CanvasRenderingContext2D *that, double x, double y, double width, double height)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').clearRect($1, $2, $3, $4);
    },
           that->privado.canvas->privado.id, x, y, width, height);
}
static void context2d_fillRect(CanvasRenderingContext2D *that, double x, double y, double width, double height)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').fillRect($1, $2, $3, $4);
    },
           that->privado.canvas->privado.id, x, y, width, height);
}
static void context2d_strokeRect(CanvasRenderingContext2D *that, double x, double y, double width, double height)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').strokeRect($1, $2, $3, $4);
    },
           that->privado.canvas->privado.id, x, y, width, height);
}
static void context2d_fillText(CanvasRenderingContext2D *that, char const *text, double x, double y, double maxWidth)
{
    if (maxWidth < 0.0)
    {
        EM_ASM({
            document.getElementById(UTF8ToString($0)).getContext('2d').fillText(UTF8ToString($1), $2, $3);
        },
               that->privado.canvas->privado.id, text, x, y);
    }
    else
    {
        EM_ASM({
            document.getElementById(UTF8ToString($0)).getContext('2d').fillText(UTF8ToString($1), $2, $3, $4);
        },
               that->privado.canvas->privado.id, text, x, y, maxWidth);
    }
}
static void context2d_strokeText(CanvasRenderingContext2D *that, char const *text, double x, double y, double maxWidth)
{
    if (maxWidth < 0.0)
    {
        EM_ASM({
            document.getElementById(UTF8ToString($0)).getContext('2d').strokeText(UTF8ToString($1), $2, $3);
        },
               that->privado.canvas->privado.id, text, x, y);
    }
    else
    {
        EM_ASM({
            document.getElementById(UTF8ToString($0)).getContext('2d').strokeText(UTF8ToString($1), $2, $3, $4);
        },
               that->privado.canvas->privado.id, text, x, y, maxWidth);
    }
}
static void context2d_setLineWidth(CanvasRenderingContext2D *that, double value)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').lineWidth = ($1);
    },
           that->privado.canvas->privado.id, value);
}
static double context2d_getLineWidth(CanvasRenderingContext2D *that)
{
    return EM_ASM_DOUBLE({
        return document.getElementById(UTF8ToString($0)).getContext('2d').lineWidth;
    },
                         that->privado.canvas->privado.id);
}
static void context2d_setLineCap(CanvasRenderingContext2D *that, char const *type)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').lineCap = UTF8ToString($1);
    },
           that->privado.canvas->privado.id, type);
}
static char const *context2d_getLineCap(CanvasRenderingContext2D *that)
{
    if (that->privado.lineCap)
        free(that->privado.lineCap);
    that->privado.lineCap = (char *)EM_ASM_INT({
        var string = document.getElementById(UTF8ToString($0)).getContext('2d').lineCap;
        var strlen = lengthBytesUTF8(string) + 1;
        var strptr = _malloc(strlen);
        stringToUTF8(string, strptr, strlen);
        return strptr;
    },
                                               that->privado.canvas->privado.id);
    return that->privado.lineCap;
}
static void context2d_setLineJoin(CanvasRenderingContext2D *that, char const *type)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').lineJoin = UTF8ToString($1);
    },
           that->privado.canvas->privado.id, type);
}
static char const *context2d_getLineJoin(CanvasRenderingContext2D *that)
{
    if (that->privado.lineJoin)
        free(that->privado.lineJoin);
    that->privado.lineJoin = (char *)EM_ASM_INT({
        var string = document.getElementById(UTF8ToString($0)).getContext('2d').lineJoin;
        var strlen = lengthBytesUTF8(string) + 1;
        var strptr = _malloc(strlen);
        stringToUTF8(string, strptr, strlen);
        return strptr;
    },
                                                that->privado.canvas->privado.id);
    return that->privado.lineJoin;
}
static char const *context2d_getFont(CanvasRenderingContext2D *that)
{
    if (that->privado.font)
        free(that->privado.font); // this field could be reused, but we won't just in case it changes from the JS side
    that->privado.font = (char *)EM_ASM_INT({
        var string = document.getElementById(UTF8ToString($0)).getContext('2d').font;
        var strlen = lengthBytesUTF8(string) + 1;
        var strptr = _malloc(strlen);
        stringToUTF8(string, strptr, strlen);
        return strptr;
    },
                                            that->privado.canvas->privado.id);
    return that->privado.font;
}
static void context2d_setFont(CanvasRenderingContext2D *that, char const *value)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').font = UTF8ToString($1);
    },
           that->privado.canvas->privado.id, value);
}
static char const *context2d_getTextAlign(CanvasRenderingContext2D *that)
{
    if (that->privado.textAlign)
        free(that->privado.textAlign);
    that->privado.textAlign = (char *)EM_ASM_INT({
        var string = document.getElementById(UTF8ToString($0)).getContext('2d').textAlign;
        var strlen = lengthBytesUTF8(string) + 1;
        var strptr = _malloc(strlen);
        stringToUTF8(string, strptr, strlen);
        return strptr;
    },
                                                 that->privado.canvas->privado.id);
    return that->privado.textAlign;
}
static void context2d_setTextAlign(CanvasRenderingContext2D *that, char const *value)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').textAlign = UTF8ToString($1);
    },
           that->privado.canvas->privado.id, value);
}
static char const *context2d_getFillStyle(CanvasRenderingContext2D *that)
{
    if (that->privado.fillStyle)
        free(that->privado.fillStyle);
    that->privado.fillStyle = (char *)EM_ASM_INT({
        var string = document.getElementById(UTF8ToString($0)).getContext('2d').fillStyle;
        var strlen = lengthBytesUTF8(string) + 1;
        var strptr = _malloc(strlen);
        stringToUTF8(string, strptr, strlen);
        return strptr;
    },
                                                 that->privado.canvas->privado.id);
    return that->privado.fillStyle;
}
static void context2d_setFillStyle(CanvasRenderingContext2D *that, char const *value)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').fillStyle = UTF8ToString($1);
    },
           that->privado.canvas->privado.id, value);
}
static char const *context2d_getStrokeStyle(CanvasRenderingContext2D *that)
{
    if (that->privado.strokeStyle)
        free(that->privado.strokeStyle);
    that->privado.strokeStyle = (char *)EM_ASM_INT({
        var string = document.getElementById(UTF8ToString($0)).getContext('2d').strokeStyle;
        var strlen = lengthBytesUTF8(string) + 1;
        var strptr = _malloc(strlen);
        stringToUTF8(string, strptr, strlen);
        return strptr;
    },
                                                   that->privado.canvas->privado.id);
    return that->privado.strokeStyle;
}
static void context2d_setStrokeStyle(CanvasRenderingContext2D *that, char const *value)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').strokeStyle = UTF8ToString($1);
    },
           that->privado.canvas->privado.id, value);
}
static void context2d_beginPath(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').beginPath();
    },
           that->privado.canvas->privado.id);
}
static void context2d_closePath(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').closePath();
    },
           that->privado.canvas->privado.id);
}
static void context2d_moveTo(CanvasRenderingContext2D *that, double x, double y)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').moveTo($1, $2);
    },
           that->privado.canvas->privado.id, x, y);
}
static void context2d_lineTo(CanvasRenderingContext2D *that, double x, double y)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').lineTo($1, $2);
    },
           that->privado.canvas->privado.id, x, y);
}
static void context2d_bezierCurveTo(CanvasRenderingContext2D *that, double cp1x, double cp1y, double cp2x, double cp2y, double x, double y)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').bezierCurveTo($1, $2, $3, $4, $5, $6);
    },
           that->privado.canvas->privado.id, cp1x, cp1y, cp2x, cp2y, x, y);
}
static void context2d_quadraticCurveTo(CanvasRenderingContext2D *that, double cpx, double cpy, double x, double y)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').quadraticCurveTo($1, $2, $3, $4);
    },
           that->privado.canvas->privado.id, cpx, cpy, x, y);
}
static void context2d_arc(CanvasRenderingContext2D *that, double x, double y, double radius, double startAngle, double endAngle)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').arc($1, $2, $3, $4, $5);
    },
           that->privado.canvas->privado.id, x, y, radius, startAngle, endAngle);
}
static void context2d_arcTo(CanvasRenderingContext2D *that, double x1, double y1, double x2, double y2, double radius)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').arcTo($1, $2, $3, $4, $5);
    },
           that->privado.canvas->privado.id, x1, y1, x2, y2, radius);
}
static void context2d_ellipse(CanvasRenderingContext2D *that, double x, double y, double radiusX, double radiusY, double rotation, double startAngle, double endAngle)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').ellipse($1, $2, $3, $4, $5, $6, $7);
    },
           that->privado.canvas->privado.id, x, y, radiusX, radiusY, rotation, startAngle, endAngle);
}
static void context2d_rect(CanvasRenderingContext2D *that, double x, double y, double width, double height)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').rect($1, $2, $3, $4);
    },
           that->privado.canvas->privado.id, x, y, width, height);
}
static void context2d_fill(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').fill();
    },
           that->privado.canvas->privado.id);
}
static void context2d_stroke(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').stroke();
    },
           that->privado.canvas->privado.id);
}
static void context2d_clip(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').clip();
    },
           that->privado.canvas->privado.id);
}
static int context2d_isPointInPath(CanvasRenderingContext2D *that, double x, double y)
{
    return EM_ASM_INT({
        return document.getElementById(UTF8ToString($0)).getContext('2d').isPointInPath($1, $2);
    },
                      that->privado.canvas->privado.id, x, y);
}
static int context2d_isPointInStroke(CanvasRenderingContext2D *that, double x, double y)
{
    return EM_ASM_INT({
        return document.getElementById(UTF8ToString($0)).getContext('2d').isPointInStroke($1, $2);
    },
                      that->privado.canvas->privado.id, x, y);
}
static void context2d_rotate(CanvasRenderingContext2D *that, double angle)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').rotate($1);
    },
           that->privado.canvas->privado.id, angle);
}
static void context2d_scale(CanvasRenderingContext2D *that, double x, double y)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').scale($1, $2);
    },
           that->privado.canvas->privado.id, x, y);
}
static void context2d_translate(CanvasRenderingContext2D *that, double x, double y)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').translate($1, $2);
    },
           that->privado.canvas->privado.id, x, y);
}
static void context2d_transform(CanvasRenderingContext2D *that, double a, double b, double c, double d, double e, double f)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').transform($1, $2, $3, $4, $5, $6);
    },
           that->privado.canvas->privado.id, a, b, c, d, e, f);
}
static void context2d_setTransform(CanvasRenderingContext2D *that, double a, double b, double c, double d, double e, double f)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').setTransform($1, $2, $3, $4, $5, $6);
    },
           that->privado.canvas->privado.id, a, b, c, d, e, f);
}
static void context2d_resetTransform(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').resetTransform();
    },
           that->privado.canvas->privado.id);
}
static void context2d_setGlobalAlpha(CanvasRenderingContext2D *that, double value)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').globalAlpha = $1;
    },
           that->privado.canvas->privado.id, value);
}
static double context2d_getGlobalAlpha(CanvasRenderingContext2D *that)
{
    return EM_ASM_DOUBLE({
        return document.getElementById(UTF8ToString($0)).getContext('2d').globalAlpha;
    },
                         that->privado.canvas->privado.id);
}
static void context2d_setGlobalCompositeOperation(CanvasRenderingContext2D *that, char const *value)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').globalCompositeOperation = $1;
    },
           that->privado.canvas->privado.id, value);
}
static char const *context2d_getGlobalCompositeOperation(CanvasRenderingContext2D *that)
{
    if (that->privado.globalCompositeOperation)
        free(that->privado.globalCompositeOperation);
    that->privado.globalCompositeOperation = (char *)EM_ASM_INT({
        var string = document.getElementById(UTF8ToString($0)).getContext('2d').globalCompositeOperation;
        var strlen = lengthBytesUTF8(string) + 1;
        var strptr = _malloc(strlen);
        stringToUTF8(string, strptr, strlen);
        return strptr;
    },
                                                                that->privado.canvas->privado.id);
    return that->privado.globalCompositeOperation;
}
static void context2d_save(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').save();
    },
           that->privado.canvas->privado.id);
}
static void context2d_restore(CanvasRenderingContext2D *that)
{
    EM_ASM({
        document.getElementById(UTF8ToString($0)).getContext('2d').restore();
    },
           that->privado.canvas->privado.id);
}
static HTMLCanvasElement *context2d_getCanvas(CanvasRenderingContext2D *that)
{
    return that->privado.canvas;
}
/* End: CanvasRenderingContext2D static methods */

static CanvasRenderingContext2D *createContext(HTMLCanvasElement *canvas, char const *contextType)
{
    if (strcmp(contextType, "2d") != 0)
        return NULL;
    CanvasRenderingContext2D *ctx = (CanvasRenderingContext2D *)malloc(sizeof(CanvasRenderingContext2D));
    /* Begin: set pseudo-privado fields */
    ctx->privado.canvas = canvas;
    strcpy(ctx->privado.contextType, contextType); // string field is a static length, no need to allocate
    ctx->privado.font = NULL;
    ctx->privado.textAlign = NULL;
    ctx->privado.fillStyle = NULL;
    ctx->privado.strokeStyle = NULL;
    ctx->privado.lineCap = NULL;
    ctx->privado.lineJoin = NULL;
    ctx->privado.globalCompositeOperation = NULL;
    /* End: set pseudo-privado fields */
    ctx->clearRect = context2d_clearRect;
    ctx->fillRect = context2d_fillRect;
    ctx->strokeRect = context2d_strokeRect;
    ctx->fillText = context2d_fillText;
    ctx->strokeText = context2d_strokeText;
    ctx->setLineWidth = context2d_setLineWidth;
    ctx->getLineWidth = context2d_getLineWidth;
    ctx->setLineCap = context2d_setLineCap;
    ctx->getLineCap = context2d_getLineCap;
    ctx->setLineJoin = context2d_setLineJoin;
    ctx->getLineJoin = context2d_getLineJoin;
    ctx->setFont = context2d_setFont;
    ctx->getFont = context2d_getFont;
    ctx->setTextAlign = context2d_setTextAlign;
    ctx->getTextAlign = context2d_getTextAlign;
    ctx->setFillStyle = context2d_setFillStyle;
    ctx->getFillStyle = context2d_getFillStyle;
    ctx->setStrokeStyle = context2d_setStrokeStyle;
    ctx->getStrokeStyle = context2d_getStrokeStyle;
    ctx->beginPath = context2d_beginPath;
    ctx->closePath = context2d_closePath;
    ctx->moveTo = context2d_moveTo;
    ctx->lineTo = context2d_lineTo;
    ctx->bezierCurveTo = context2d_bezierCurveTo;
    ctx->quadraticCurveTo = context2d_quadraticCurveTo;
    ctx->arc = context2d_arc;
    ctx->arcTo = context2d_arcTo;
    ctx->ellipse = context2d_ellipse;
    ctx->rect = context2d_rect;
    ctx->fill = context2d_fill;
    ctx->stroke = context2d_stroke;
    ctx->clip = context2d_clip;
    ctx->isPointInPath = context2d_isPointInPath;
    ctx->isPointInStroke = context2d_isPointInStroke;
    ctx->rotate = context2d_rotate;
    ctx->scale = context2d_scale;
    ctx->translate = context2d_translate;
    ctx->transform = context2d_transform;
    ctx->setTransform = context2d_setTransform;
    ctx->resetTransform = context2d_resetTransform;
    ctx->setGlobalAlpha = context2d_setGlobalAlpha;
    ctx->getGlobalAlpha = context2d_getGlobalAlpha;
    ctx->setGlobalCompositeOperation = context2d_setGlobalCompositeOperation;
    ctx->getGlobalCompositeOperation = context2d_getGlobalCompositeOperation;
    ctx->save = context2d_save;
    ctx->restore = context2d_restore;
    ctx->getCanvas = context2d_getCanvas;
    return ctx;
}

void freeCanvas(HTMLCanvasElement *canvas)
{
    if (canvas)
    {
        free(canvas->privado.id);
        if (canvas->privado.ctx)
        {
            if (canvas->privado.ctx->privado.font)
                free(canvas->privado.ctx->privado.font);
            if (canvas->privado.ctx->privado.textAlign)
                free(canvas->privado.ctx->privado.textAlign);
            if (canvas->privado.ctx->privado.fillStyle)
                free(canvas->privado.ctx->privado.fillStyle);
            if (canvas->privado.ctx->privado.strokeStyle)
                free(canvas->privado.ctx->privado.strokeStyle);
            if (canvas->privado.ctx->privado.lineCap)
                free(canvas->privado.ctx->privado.lineCap);
            if (canvas->privado.ctx->privado.lineJoin)
                free(canvas->privado.ctx->privado.lineJoin);
            if (canvas->privado.ctx->privado.globalCompositeOperation)
                free(canvas->privado.ctx->privado.globalCompositeOperation);
            free(canvas->privado.ctx);
        }
        free(canvas);
    }
}
