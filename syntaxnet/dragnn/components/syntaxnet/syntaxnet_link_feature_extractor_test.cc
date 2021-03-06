#include "dragnn/components/syntaxnet/syntaxnet_link_feature_extractor.h"

#include <string>

#include "dragnn/core/test/generic.h"
#include "dragnn/protos/spec.pb.h"
#include "syntaxnet/task_context.h"
#include "tensorflow/core/platform/test.h"

using syntaxnet::test::EqualsProto;

namespace syntaxnet {
namespace dragnn {

class ExportSpecTest : public ::testing::Test {
 public:
};

TEST_F(ExportSpecTest, WritesChannelSpec) {
  TaskContext context;

  context.SetParameter("neurosis_feature_syntax_version", "2");
  context.SetParameter("link_features", "input.focus;stack.focus");
  context.SetParameter("link_embedding_names", "tagger;parser");
  context.SetParameter("link_predicate_maps", "none;none");
  context.SetParameter("link_embedding_dims", "16;16");
  context.SetParameter("link_source_components", "tagger;parser");
  context.SetParameter("link_source_layers", "hidden0;lstm");
  context.SetParameter("link_source_translators", "token;last_action");

  SyntaxNetLinkFeatureExtractor link_features;
  link_features.Setup(&context);
  link_features.Init(&context);

  ComponentSpec spec;
  link_features.AddLinkedFeatureChannelProtos(&spec);
  const string expected_spec_str = R"(
    linked_feature {
      name: "tagger"
      fml: "input.focus"
      embedding_dim: 16
      size: 1
      source_component: "tagger"
      source_translator: "token"
      source_layer: "hidden0"
    }
    linked_feature {
      name: "parser"
      fml: "stack.focus"
      embedding_dim: 16
      size: 1
      source_component: "parser"
      source_translator: "last_action"
      source_layer: "lstm"
    }
  )";
  ComponentSpec expected_spec;
  TextFormat::ParseFromString(expected_spec_str, &expected_spec);
  EXPECT_THAT(spec, EqualsProto(expected_spec));
}

}  // namespace dragnn
}  // namespace syntaxnet
