#ifndef UNIGD_EXTERNAL_API_V1_H
#define UNIGD_EXTERNAL_API_V1_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

    typedef void *UNIGD_HANDLE;
    typedef void *UNIGD_RENDER_HANDLE;
    typedef void *UNIGD_RENDERERS_HANDLE;
    typedef void *UNIGD_RENDERERS_ENTRY_HANDLE;
    typedef void *UNIGD_FIND_HANDLE;
    typedef const char *UNIGD_RENDERER_ID;
    typedef uint32_t UNIGD_PLOT_ID;
    typedef uint32_t UNIGD_PLOT_INDEX;
    typedef int32_t UNIGD_PLOT_RELATIVE;

    struct unigd_graphics_client
    {
        void (*start)(void *);
        void (*close)(void *);
        void (*state_change)(void *);
        int (*client_id)(void *);
        // int (client_status)(char *);
    };

    struct unigd_renderer_info
    {
        UNIGD_RENDERER_ID id;
        const char *mime;
        const char *fileext;
        const char *name;
        const char *type;
        const char *description;
        bool text;
    };

    struct unigd_renderers_list
    {
        const unigd_renderer_info *entries;
        uint64_t size;
    };

    struct unigd_device_state
    {
        int upid;
        UNIGD_PLOT_INDEX hsize;
        bool active;
    };

    struct unigd_render_args
    {
        double width;
        double height;
        double scale;
    };

    struct unigd_render_access
    {
        const uint8_t *buffer;
        uint64_t size;
    };

    struct unigd_find_results
    {
        unigd_device_state state;
        UNIGD_PLOT_INDEX size;
        UNIGD_PLOT_ID *ids;
    };

    // unigd API access version 1
    struct unigd_api_v1
    {
        // GENERAL

        // Send a log message to R (thread safe).
        void (*log)(const char *t_message);

        // DEVICE

        // Attach a client to the unigd device.
        UNIGD_HANDLE(*device_attach)
        (int, unigd_graphics_client *, void *);

        // Get client.
        void *(*device_get)(int devnum, int client_id);

        // Destroy device handle.
        void (*device_destroy)(UNIGD_HANDLE);

        // Get the current unigd device state.
        unigd_device_state (*device_state)(UNIGD_HANDLE);

        // RENDERING

        // Render a plot.
        UNIGD_RENDER_HANDLE(*device_render_create)
        (UNIGD_HANDLE, UNIGD_RENDERER_ID, UNIGD_PLOT_ID, unigd_render_args, unigd_render_access *);

        // Free render memory.
        void (*device_render_destroy)(UNIGD_RENDER_HANDLE);

        // HISTORY

        // Remove a plot from history.
        bool (*device_plots_remove)(UNIGD_HANDLE, UNIGD_PLOT_ID);

        // Clear plot history.
        bool (*device_plots_clear)(UNIGD_HANDLE);

        // Plot ID lookup.
        UNIGD_FIND_HANDLE(*device_plots_find)
        (UNIGD_HANDLE, unigd_find_results *results);
        // Plot ID lookup.
        UNIGD_FIND_HANDLE(*device_plots_find_index)
        (UNIGD_HANDLE, UNIGD_PLOT_RELATIVE index, unigd_find_results *results);
        // Plot ID lookup.
        UNIGD_FIND_HANDLE(*device_plots_find_range)
        (UNIGD_HANDLE, UNIGD_PLOT_RELATIVE offset, UNIGD_PLOT_INDEX limit, unigd_find_results *results);

        // Free plot ID lookup memory.
        void (*device_plots_find_destroy)(UNIGD_FIND_HANDLE);

        // RENDERERS

        // Get full list of available renderers.
        UNIGD_RENDERERS_HANDLE(*renderers)
        (unigd_renderers_list *renderer);

        // Free memory of renderers list.
        void (*renderers_destroy)(UNIGD_RENDERERS_HANDLE);

        // Renderer ID lookup.
        UNIGD_RENDERERS_ENTRY_HANDLE(*renderers_find)
        (UNIGD_RENDERER_ID, unigd_renderer_info *renderer);

        // Free memory of renderer lookup.
        void (*renderers_find_destroy)(UNIGD_RENDERERS_ENTRY_HANDLE);
    };

#ifdef __cplusplus
}
#endif

#endif // UNIGD_EXTERNAL_API_V1_H
