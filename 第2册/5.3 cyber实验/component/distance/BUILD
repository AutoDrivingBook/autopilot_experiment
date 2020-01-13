load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "distance.so",
    linkopts = ["-shared"],
    linkstatic = False,
    deps = [":distance_lib"],
)

cc_library(
    name = "distance_lib",
    srcs = [
        "distance.cc",
    ],
    hdrs = [
        "distance.h",
    ],
    deps = [
        "//cyber",
        "//cyber/examples/proto:examples_cc_proto",
    ],
)

cpplint()
