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
#include "cyber/examples/component/control/control.h"
#include "cyber/time/rate.h"
#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/examples/proto/examples.pb.h"
#include "cyber/time/time.h"
#include "cyber/cyber.h"

using apollo::cyber::Rate;
using apollo::cyber::Time;
using apollo::cyber::examples::proto::EChatter;

bool control::Init() {
  AINFO << "control component init";
  control_writer_ = node_->CreateWriter<EChatter>("/carstatus/control");
  return true;
}

bool control::Proc(const std::shared_ptr<EChatter>& msg0,
                               const std::shared_ptr<EChatter>& msg1) {
  AINFO << "Start control component Proc [" << msg0->content() << "] ["
        << msg1->content() << "]";
  auto lt = msg0->lidar_timestamp();
  auto sid = msg0->seq();
  // proc time stamp
  auto ts = apollo::cyber::Time::Now().ToNanosecond();
  // end_to_end latency
  auto e2e = ts - lt;
  AINFO << "[control] seq_id: " << sid
        << ", e2e latency: " << e2e;

  auto out_msg = std::make_shared<EChatter>();
  if(msg0->content()>0||msg1->content()>0) {
    out_msg->set_content(1);
  }
  else {
    out_msg->set_content(1);  
  }

  out_msg->set_timestamp(ts);
  out_msg->set_seq(sid);
  out_msg->set_lidar_timestamp(lt);
  control_writer_->Write(out_msg);

  AINFO << "control: Write drivermsg->"
        << out_msg->content();
  return true;
}
