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
#include "cyber/examples/component/cal1/cal1.h"
#include "cyber/time/rate.h"
#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/time/time.h"
#include "cyber/cyber.h"
#include "cyber/examples/proto/examples.pb.h"

using apollo::cyber::Rate;
using apollo::cyber::Time;
using apollo::cyber::examples::proto::EChatter;

bool cal1::Init() {
  AINFO << "cal1 component init";
  cal1_writer_ = node_->CreateWriter<EChatter>("/carstatus/speed2");
  return true;
}

bool cal1::Proc(const std::shared_ptr<EChatter>& msg0) {
  AINFO << "Start cal1 component Proc [" << msg0->content() << "]";
  // time_stamp
  auto lt = msg0->lidar_timestamp();
  // sequence id
  auto sid = msg0->seq();

  auto out_msg = std::make_shared<EChatter>();
  if(msg0->content()>100) {
    out_msg->set_content(1);	
  } else {
    out_msg->set_content(0);
  }

  int a = 0;
  for (int i = 0; i < 10000000; ++i) {
    a += 1;
  }

  // proc time stamp
  auto ts = apollo::cyber::Time::Now().ToNanosecond();
  out_msg->set_timestamp(ts);
  out_msg->set_lidar_timestamp(lt);
  out_msg->set_seq(sid);
  cal1_writer_->Write(out_msg);
  AINFO << "cal1: Write drivermsg->"
        << out_msg->content();
  return true;
}
