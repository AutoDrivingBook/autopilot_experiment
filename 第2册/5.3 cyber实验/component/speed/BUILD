load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "speed.so",
    linkopts = ["-shared"],
    linkstatic = False,
    deps = [":speed_lib"],
)

cc_library(
    name = "speed_lib",
    srcs = [
        "speed.cc",
    ],
    hdrs = [
        "speed.h",
    ],
    deps = [
        "//cyber",
        "//cyber/examples/proto:examples_cc_proto",
    ],
)

cpplint()
