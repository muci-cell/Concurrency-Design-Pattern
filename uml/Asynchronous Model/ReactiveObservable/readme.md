这个对应响应式编程的核心

Reactive/Observable 模型更适合处理**连续的数据流**或**实时事件**，适合需要多次更新的场景，如：

* **实时数据流**：如股票行情、传感器数据、流媒体数据。
* **用户交互**：在前端开发中，用户的每一次点击或操作都可以作为事件流的一部分。
* **异步系统通讯**：在微服务架构中，Reactive 模型常用于处理异步事件流，如消息队列、流式数据处理等。

### Reactive/Observable 关键概念

* **Observable**：表示数据源，可以持续产生值。类似于一个管道，任何订阅它的 Observer 都会获得更新的数据。
* **Observer**：观察者，订阅 Observable，以便在数据发生变化时自动接收更新。
* **Operators**：用于对数据流进行变换、过滤或组合的操作，如 `map`、`filter`、`merge` 等。
* **Scheduler**：负责控制 Observable 的执行线程，可以指定在哪个线程中处理数据流。

以下是ReactiveX给的规范：   ~~注意,仅供参考这里只是想说明响应式编程博大精深~~

### Observable 契约总结

#### 通知

一个 **Observable**（可观察对象）通过以下几种通知与其观察者进行通信：

1. **OnNext**：传递一个由 Observable 发出的项给观察者。
2. **OnCompleted**：表明 Observable 成功完成并且不会再发出任何项。
3. **OnError**：表示 Observable 因特定错误条件终止，且不会再发出任何项。
4. **OnSubscribe**（可选）：表示 Observable 准备接受观察者的 **Request**（请求）通知（见下文的背压机制）。

一个 **Observer**（观察者）通过以下几种通知与其 Observable 进行通信：

1. **Subscribe**：表示观察者已准备好接收来自 Observable 的通知。
2. **Unsubscribe**：表示观察者不再希望接收来自 Observable 的通知。
3. **Request**（可选）：表示观察者希望 Observable 发出的 OnNext 通知的数量不超过特定数目（见下文的背压机制）。

#### 通知的契约

一个 Observable 可以发出零次或多次 **OnNext** 通知，每次通知代表一个被发出的项；之后，它可能跟随发出一个 **OnCompleted** 或 **OnError** 通知，但不能两者兼具。一旦发出 **OnCompleted** 或 **OnError** 通知，它将不再发出任何通知。

- Observable 可以不发出任何项。
- Observable 也可以永不通过 **OnCompleted** 或 **OnError** 通知终止。这意味着 Observable 可以仅发出 OnNext 通知，或仅发出 OnCompleted 或 OnError 通知，甚至不发出任何通知。

Observable 必须按顺序（而不是并行）向观察者发出通知。这些通知可以来自不同线程，但必须遵守线程间的**先发生关系**（happens-before relationship）。

#### Observable 的终止

如果一个 Observable 尚未发出 **OnCompleted** 或 **OnError** 通知，观察者可以认为它仍然是活跃的（即使它当前未发出项），并可以继续向其发送通知（例如 **Unsubscribe** 或 **Request** 通知）。当 Observable 发出 OnCompleted 或 OnError 通知后，它可以释放其资源并终止，其观察者也不应再试图与其通信。

- **OnError** 通知必须包含错误的具体原因（即，不允许传入空值作为 OnError 的参数）。

在一个 Observable 终止之前，它必须首先向所有已订阅的观察者发出 **OnCompleted** 或 **OnError** 通知。

#### 订阅和取消订阅

当一个观察者通过发送 **Subscribe** 通知向 Observable 订阅后，Observable 可能会立即向该观察者发送通知。

当观察者向 Observable 发送 **Unsubscribe** 通知后，Observable 将尝试停止向该观察者发出通知。不过，无法保证在 Unsubscribe 之后立即停止发送通知。

当 Observable 向观察者发出 OnError 或 OnCompleted 通知后，订阅即被终止。观察者在此情境下无需再发送 Unsubscribe 通知来终止订阅。

#### 多观察者

如果在 Observable 已经向第一个观察者发出项后，第二个观察者订阅该 Observable，Observable 可以选择以下几种行为：

1. 向每个观察者发出相同的项；
2. 向第二个观察者重新发出从头开始的完整序列；
3. 向第二个观察者发出一个完全不同的项序列。

对于同一个 Observable 的两个观察者而言，无法保证它们会看到相同的项序列。

#### 背压机制（Backpressure）

背压机制是可选的，并不是所有的 ReactiveX 实现都包含背压机制；即使包含，也并非所有 Observable 或操作符都支持背压。

如果一个 Observable 实现了背压机制，并且其观察者采用了背压机制，那么 Observable 不会在订阅后立即向观察者发出项，而是会向观察者发出 **OnSubscribe** 通知。

观察者在接收 OnSubscribe 通知后，可以随时向其订阅的 Observable 发出 **Request** 通知，用以请求指定数量的项。Observable 会响应此请求，最多发出指定数量的项给观察者。此外，Observable 也可能会发出 **OnCompleted** 或 **OnError** 通知，甚至可能在观察者请求任何项之前发出这些终止通知。

一个不支持背压的 Observable 在接收到 Request 通知后，应发出 **OnError** 通知，表明其不支持背压。

Request 通知是**累加**的，例如，若观察者连续三次向 Observable 发出数量分别为 3、5 和 10 的 Request 通知，则 Observable 最多可以向观察者发出 18 个项，而不论这些 Request 通知的顺序如何。

如果 Observable 生成的项数多于观察者请求的项数，Observable 可以选择丢弃多余项、暂存这些项以便日后发送，或者使用其他策略处理溢出。
