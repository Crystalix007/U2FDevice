#pragma once

template <typename InContainer, typename OutContainer>
void b64encode(const InContainer &iContainer, OutContainer &oContainer);

template <typename InContainer, typename OutContainer>
void b64decode(const InContainer &container, OutContainer &oContainer);

template <typename InContainer, typename Elem, size_t count>
void b64encode(const InContainer &iContainer, std::array<Elem, count> &oArr);

template <typename InContainer, typename Elem, size_t count>
void b64decode(const InContainer &iContainer, std::array<Elem, count> &oArr);

template <typename InContainerIter, typename OutContainerIter>
void b64encode(const InContainerIter beginIter, const InContainerIter endIter, OutContainerIter oBeginIter);

template <typename InContainerIter, typename OutContainerIter>
void b64decode(const InContainerIter beginIter, const InContainerIter endIter, OutContainerIter oBeginIter);
