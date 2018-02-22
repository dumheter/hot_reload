// ============================================================ //
// Headers
// ============================================================ //

#include "thirdparty/cr/cr.h"
#include <cassert>
#include <cstdint>

// ============================================================ //
// Data types
// ============================================================ //

struct Host_data {
  int a = 0;;
  int b = 0;;
  int result = 0;
};

static Host_data* m_data;
static unsigned int CR_STATE m_version = 0;
static uint32_t m_failure = 0;

// ============================================================ //
// Functions
// ============================================================ //

int qadd(const int a, const int b) {
  return 1337;
}

// ============================================================ //
// Main
// ============================================================ //

CR_EXPORT int cr_main(struct cr_plugin* ctx, enum cr_op operation) {
  assert(ctx);
  m_data = (Host_data *)ctx->userdata;
  m_version = ctx->version;
  m_failure = ctx->failure;

  switch (operation) {
  case CR_LOAD:
    //init();
    return 0;
  case CR_UNLOAD:
    // if needed, save stuff to pass over to next instance
    return 0;
  case CR_CLOSE:
    //shutdown();
    return 0;
  case CR_STEP:
    ((Host_data*)ctx->userdata)->result = qadd(((Host_data*)ctx->userdata)->a, ((Host_data*)ctx->userdata)->b);
    return 0;
  }

  return 0;
}
