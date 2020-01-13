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
#include "cyber/examples/component/cal2/cal2.h"
#include "cyber/time/rate.h"
#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/time/time.h"
#include "cyber/cyber.h"
#include "cyber/examples/proto/examples.pb.h"


using apollo::cyber::Rate;
using apollo::cyber::Time;
using apollo::cyber::examples::proto::EChatter;

bool cal2::Init() {
  AINFO << "cal2 component init";
  cal2_writer_ = node_->CreateWriter<EChatter>("/carstatus/distance2");
  return true;
}

bool cal2::Proc(const std::shared_ptr<EChatter>& msg0,
                               const std::shared_ptr<EChatter>& msg1) {
  AINFO << "Start cal2 component Proc [" << msg0->content() << "] ["
        << msg1->content() << "]";
  static int i = 0;
  auto out_msg = std::make_shared<EChatter>();
  // proc time stamp
 // auto ts = apollo::cyber::Time::Now().ToNanoSecond();
if(msg0->content()>60&&msg1->content()<80) {
  out_msg->set_content(1);
}
else {
    out_msg->set_content(0);  
}

  out_msg->set_timestamp(i++);
  cal2_writer_->Write(out_msg);
  AINFO << "cal2: Write drivermsg->"
        << out_msg->content();
  return true;
}
