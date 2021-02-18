#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/memory/stl/SimpleStringStream.h>

using namespace aws::lambda_runtime;

static invocation_response my_handler(invocation_request const& request)
{

    using namespace Aws::Utils::Json;

    JsonValue json_request(request.payload);
    if (!json_request.WasParseSuccessful()) {
        return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
    }

    auto request_view = json_request.View();

    if (request_view.ValueExists("location")) {
        JsonValue response;
        response.WithString("given_loaction", request_view.GetString("location"));
        return invocation_response::success(response.View().WriteCompact(), "application/json");
    } else {
        return invocation_response::failure("'location' not in payload", "InvalidJSON");
    }
}

int main()
{
    run_handler(my_handler);
    return 0;
}
