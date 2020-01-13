load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "control.so",
    linkopts = ["-shared"],
    linkstatic = False,
    deps = [":control_lib"],
)

cc_library(
    name = "control_lib",
    srcs = [
        "control.cc",
    ],
    hdrs = [
        "control.h",
    ],
    deps = [
        "//cyber",
        "//cyber/examples/proto:examples_cc_proto",
    ],
)

cpplint()
