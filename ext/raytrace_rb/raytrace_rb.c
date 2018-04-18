#include <ruby.h>

VALUE RaytraceRb = Qnil;

void Init_raytrace_rb();
VALUE method_raytrace_rb_say_hello(VALUE, VALUE);

void Init_raytrace_rb() {
  RaytraceRb = rb_define_module("RaytraceRb");
  rb_define_singleton_method(RaytraceRb, "say_hello", method_raytrace_rb_say_hello, 1);
}

VALUE method_raytrace_rb_say_hello(VALUE self, VALUE argument) {
  VALUE prefix = rb_str_new_cstr("Hello ");
  char *argumentString = StringValueCStr(argument);
  return rb_str_cat2(prefix, argumentString);
}
