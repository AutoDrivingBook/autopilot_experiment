/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "cyber/examples/component/speed/speed.h"

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/examples/proto/examples.pb.h"

bool speed::Init() {
  speed_writer_ = node_->CreateWriter<EChatter>("/carstatus/speed1");
  return true;
}

bool speed::Proc() {
  static int i = 0;
  auto out_msg = std::make_shared<EChatter>();
  // current time
  auto cur_time = apollo::cyber::Time::Now().ToNanosecond();
  out_msg->set_timestamp(cur_time);
  out_msg->set_lidar_timestamp(cur_time);
  out_msg->set_seq(i++);
  out_msg->set_content(70);
  speed_writer_->Write(out_msg);
  AINFO << "speed: Write drivermsg->"
        << out_msg->content();
  return true;
}
