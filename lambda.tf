resource "aws_lambda_function" "iot_processor" {
  function_name = "${var.project}-${var.env}-processor"
  role          = aws_iam_role.lambda_exec.arn
  runtime       = "python3.12"
  handler       = "lambda_function.lambda_handler"
  filename      = "lambda.zip"
}

resource "aws_lambda_permission" "allow_iot" {
  statement_id  = "AllowExecutionFromIoT"
  action        = "lambda:InvokeFunction"
  function_name = aws_lambda_function.iot_processor.function_name
  principal     = "iot.amazonaws.com"
}

resource "aws_iot_topic_rule" "to_lambda" {
  name        ="${replace(var.project, "-", "_")}_${var.env}_rule"
  sql         = "SELECT * FROM '${var.topic_pub}'"
  sql_version = "2016-03-23"
  enabled     = true

  lambda {
    function_arn = aws_lambda_function.iot_processor.arn
  }
}
