load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "cal2.so",
    linkopts = ["-shared"],
    linkstatic = False,
    deps = [":cal2_lib"],
)

cc_library(
    name = "cal2_lib",
    srcs = [
        "cal2.cc",
    ],
    hdrs = [
        "cal2.h",
    ],
    deps = [
        "//cyber",
        "//cyber/examples/proto:examples_cc_proto",
    ],
)

cpplint()
