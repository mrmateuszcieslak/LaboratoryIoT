resource "aws_iot_thing" "thing" {
  name = var.thing_name
  attributes = {
    project = var.project
    env     = var.env
  }
}

resource "aws_iot_policy" "policy" {
  name = "${var.project}-${var.env}-${var.thing_name}-policy"

  policy = jsonencode({
    Version = "2012-10-17",
    Statement = [{
      Effect   = "Allow",
      Action   = ["iot:Connect","iot:Publish","iot:Subscribe","iot:Receive"],
      Resource = "*"
    }]
  })
}

resource "aws_iot_certificate" "cert" { active = true }

resource "aws_iot_policy_attachment" "attach" {
  policy = aws_iot_policy.policy.name
  target = aws_iot_certificate.cert.arn
}

resource "aws_iot_thing_principal_attachment" "thing_cert" {
  thing     = aws_iot_thing.thing.name
  principal = aws_iot_certificate.cert.arn
}

data "aws_iot_endpoint" "iot" { endpoint_type = "iot:Data-ATS" }
