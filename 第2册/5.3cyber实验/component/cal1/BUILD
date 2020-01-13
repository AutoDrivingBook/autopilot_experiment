load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "cal1.so",
    linkopts = ["-shared"],
    linkstatic = False,
    deps = [":cal1_lib"],
)

cc_library(
    name = "cal1_lib",
    srcs = [
        "cal1.cc",
    ],
    hdrs = [
        "cal1.h",
    ],
    deps = [
        "//cyber",
        "//cyber/examples/proto:examples_cc_proto",
    ],
)

cpplint()
